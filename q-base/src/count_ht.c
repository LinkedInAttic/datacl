#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "set_meta.h"
#include "aux_meta.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "count_ht.h"
#include "mk_temp_file.h"
#include "mix.h"

// last review 9/22/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
count_ht(
	 char *src_tbl,
	 char *fld,
	 char *cfld,
	 char *dst_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;    size_t nX = 0;
  char *cfld_X = NULL;    size_t cfld_nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  TBL_REC_TYPE src_tbl_rec; int src_tbl_id = -1; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id = -1; 
  FLD_REC_TYPE fld_rec;     int fld_id     = -1; 
  FLD_REC_TYPE nn_fld_rec;  int nn_fld_id  = -1; 
  FLD_REC_TYPE cfld_rec;    
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id = -1; 
  FLD_REC_TYPE val_fld_rec; int val_fld_id = -1; 
  FLD_REC_TYPE cnt_fld_rec; int cnt_fld_id = -1; 
  long long src_nR = 0, dst_nR = 0;
#define BUFLEN 32
  char op[BUFLEN], str_dst_nR[BUFLEN];
  char *val_X = NULL; size_t n_val_X = 0;
  char *cnt_X = NULL; size_t n_cnt_X = 0;
  // for final dumping of results
  char      *I1ptr = NULL; char      I1val; 
  short     *I2ptr = NULL; short     I2val; 
  int       *I4ptr = NULL; int       I4val; 
  long long *I8ptr = NULL; long long I8val;
  //-- for pthreads
  HT_I8_TYPE *ht = NULL;
  int sz_ht;    // size of hash table 
  int n_ht = 0;  // number of occupied entries in hash table 
  unsigned long long valUI8;
  int ht_loc, pos_ht; bool space_found, key_exists; 
  long long num_probes = 0; long long max_count = 0;
  int val_ddir_id = -1, val_fileno = -1;
  int cnt_ddir_id = -1, cnt_fileno = -1;
  int fldsz = 0; size_t filesz = 0;
  long long *ht_locs = NULL; char *is_good = NULL;

  //----------------------------------------------------------------
  // cfld can be null
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(op, BUFLEN);
  zero_string(str_dst_nR, BUFLEN);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  if ( src_tbl_id < 0 ) { go_BYE(-1); }
  src_nR = src_tbl_rec.nR;
  if ( src_nR >= INT_MAX ) { 
    fprintf(stderr, "WARNING! Overflow possible. Proceed at your peril\n"); 
  }
  status = is_fld(NULL, src_tbl_id, fld, &fld_id, &fld_rec, 
		  &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( fld_id < 0 ) { go_BYE(-1); }
  status = get_data(fld_rec, &X, &nX, 0); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, 0); cBYE(status);
    cBYE(status);
  }

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    if ( strcmp(fld, cfld) == 0 ) { go_BYE(-1); }
    status = is_fld(NULL, src_tbl_id, cfld, &fld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); 
    cBYE(status);
    if ( nn_cfld_id >= 0 ) { go_BYE(-1); } 
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }

  sz_ht = prime_geq(2*MAX_DISTINCT_ENTRIES_COUNT_HT);
  ht = (HT_I8_TYPE *)malloc(sz_ht * sizeof(HT_I8_TYPE));
  return_if_malloc_failed(ht);
  for ( int i = 0; i < sz_ht; i++ ) { 
    ht[i].key = 0;
    ht[i].cnt = 0;
  } 

  int block_size = 2*1048576; int num_blocks;
  ht_locs = malloc(block_size * sizeof(long long));
  return_if_malloc_failed(ht_locs);
  is_good = malloc(block_size * sizeof(char));
  return_if_malloc_failed(is_good);
  num_blocks = src_nR / block_size;
  if ( ( num_blocks * block_size ) < src_nR ) { 
    num_blocks++;
  }

  for ( int b = 0; b < num_blocks; b++ ) {
    long long lb = b *  block_size;
    long long ub = lb + block_size;
    if ( lb > src_nR ) { status = -1; continue; }
    if ( ub > src_nR ) { ub = src_nR; }
    long long nX = ub - lb;
    I1ptr = (char *)X;      I1ptr += lb;
    I2ptr = (short *)X;     I2ptr += lb;
    I4ptr = (int *)X;       I4ptr += lb;
    I8ptr = (long long *)X; I8ptr += lb;
#pragma cilk grainsize = 4096
    cilkfor ( long long j = 0; j < nX; j++ ) { 
      is_good[j] = 0;
      if ( ( nn_X   != NULL ) && ( nn_X[j]   == FALSE ) ) { continue; }
      if ( ( cfld_X != NULL ) && ( cfld_X[j] == FALSE ) ) { continue; }
      is_good[j] = 1;
      switch ( fld_rec.fldtype ) { 
      case I1 : I1val = I1ptr[j]; valUI8 = (unsigned long long)I1val; break;
      case I2 : I2val = I2ptr[j]; valUI8 = (unsigned long long)I2val; break;
      case I4 : I4val = I4ptr[j]; valUI8 = (unsigned long long)I4val; break;
      case I8 : I8val = I8ptr[j]; valUI8 = (unsigned long long)I8val; break;
      default : status = -1; break;
      }
      ht_locs[j] = mix_I8(valUI8) % sz_ht;
    }
    cBYE(status);
    //-----------------------------------------------
    for ( long long j = 0; j < nX; j++ ) {
      if ( is_good[j] == 0 ) { continue; }
      switch ( fld_rec.fldtype ) { 
      case I1 : I1val = I1ptr[j]; valUI8 = (unsigned long long)I1val; break;
      case I2 : I2val = I2ptr[j]; valUI8 = (unsigned long long)I2val; break;
      case I4 : I4val = I4ptr[j]; valUI8 = (unsigned long long)I4val; break;
      case I8 : I8val = I8ptr[j]; valUI8 = (unsigned long long)I8val; break;
      default : go_BYE(-1); break;
      }
      ht_loc = ht_locs[j];
      pos_ht = -1;
      // Determine whether you have seen this guy before or not
      // If not, then you also need to find the position to put him
      space_found = false; 
      for ( int i = ht_loc; i < sz_ht; i++ ) {
	num_probes++;
	if ( ht[i].key == valUI8 ) { 
	  pos_ht = i;
	  key_exists = true;
	  space_found = true;
	  break;
	}
	if ( ht[i].cnt == 0 ) { // unoccupied 
	  pos_ht = i;
	  key_exists = false;
	  space_found = true;
	  break;
	}
      }
      if ( space_found == false ) {
	for ( int i = 0; i < ht_loc; i++ ) { 
	  num_probes++;
	  if ( ht[i].key == valUI8 ) { 
	    pos_ht = i;
	    key_exists = true;
	    space_found = true;
	    break;
	  }
	  if ( ht[i].cnt == 0 ) { // unoccupied 
	    pos_ht = i;
	    key_exists = false;
	    space_found = true;
	    break;
	  }
	}
      }
      //---------------------------
      if ( space_found == false ) { go_BYE(-1); }
      if ( ( pos_ht < 0 ) || ( pos_ht >= sz_ht ) ) { go_BYE(-1); }
      //---------------------------
      if ( key_exists ) {
	if ( ht[pos_ht].key != valUI8 ) { go_BYE(-1); }
	// if ( pos_ht == 1 ) { go_BYE(-1); }
	if ( ht[pos_ht].cnt == 0 ) {
	  ht[pos_ht].cnt = 1;
	  ht[pos_ht].key = valUI8;
	  n_ht++;
	}
	ht[pos_ht].cnt = ht[pos_ht].cnt + 1;
	if ( ht[pos_ht].cnt > max_count ) { 
	  max_count = ht[pos_ht].cnt;
	}
      }
      else {
	ht[pos_ht].cnt = 1;
	ht[pos_ht].key = valUI8;
	n_ht++;
      }
      if ( n_ht > ( sz_ht >> 1 ) ) {
	fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1);
      }
    }
  }
  //-------------------------------------------------------
  dst_nR = n_ht;
  if  ( dst_nR == 0 ) { goto BYE; } // caller needs to check

  sprintf(str_dst_nR, "%lld", dst_nR);
  status = add_tbl(dst_tbl, str_dst_nR, &dst_tbl_id, &dst_tbl_rec); 
  cBYE(status);
  
  status = get_fld_sz(fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * dst_nR;
  status = mk_temp_file(filesz, &val_ddir_id, &val_fileno); cBYE(status);
  status = q_mmap(val_ddir_id, val_fileno, &val_X, &n_val_X, 1); cBYE(status);
  I4ptr = (int       *)val_X;
  I8ptr = (long long *)val_X;
  int ctr = 0;
  for ( int i = 0; i < sz_ht; i++ ) {
    if ( ht[i].cnt == 0 ) {  continue; }
    ctr++;
    switch ( fld_rec.fldtype ) { 
    case I1 : *I1ptr++ = (char     )(ht[i].key); break;
    case I2 : *I2ptr++ = (short    )(ht[i].key); break;
    case I4 : *I4ptr++ = (int      )(ht[i].key); break;
    case I8 : *I8ptr++ = (long long)(ht[i].key); break;
    default : go_BYE(-1); break;
    }
  }
  if ( ctr != n_ht ) { go_BYE(-1); }
  rs_munmap(val_X, n_val_X);
  zero_fld_rec(&val_fld_rec); val_fld_rec.fldtype = fld_rec.fldtype;
  status = add_fld(dst_tbl_id, "value", val_ddir_id, val_fileno,
		   &val_fld_id, &val_fld_rec);
  cBYE(status);

  //-------------------------------
  zero_fld_rec(&cnt_fld_rec); 
  if ( max_count <= INT_MAX ) { 
    cnt_fld_rec.fldtype = I4;
  }
  else {
    cnt_fld_rec.fldtype = I8;
  }
  status = get_fld_sz(cnt_fld_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * dst_nR;
  status = mk_temp_file(filesz, &cnt_ddir_id, &cnt_fileno); cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_fileno, &cnt_X, &n_cnt_X, 1); cBYE(status);
  I4ptr = (int *)cnt_X;
  I8ptr = (long long *)cnt_X;

  ctr = 0;
  for ( int i = 0; i < sz_ht; i++ ) {
    if ( ht[i].cnt == 0 ) { continue; }
    ctr++;
    if ( max_count <= INT_MAX ) { 
      *I4ptr++ = ht[i].cnt; 
    }
    else {
      *I8ptr++ = ht[i].cnt; 
    }
  }
  //--------------------------------------------
  if ( ctr != n_ht ) { go_BYE(-1); }
  rs_munmap(cnt_X, n_cnt_X);
  status = add_fld(dst_tbl_id, "count", cnt_ddir_id, cnt_fileno, 
		   &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
  // fprintf(stderr, "Average num probes = %lf \n", (double)num_probes/(double)src_nR);

 BYE:
  free_if_non_null(ht);
  free_if_non_null(ht_locs);
  free_if_non_null(is_good);
  rs_munmap(X, nX);
  rs_munmap(cfld_X, cfld_nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(val_X, n_val_X);
  rs_munmap(cnt_X, n_cnt_X);
  return status ;
}
