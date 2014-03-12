#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "bin_search_I4.h"
#include "bin_search_I8.h"
#include "get_nR.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "f_to_s.h"

/* This is used when f2 is sorted ascending but f1 is not */
/* Also assume that f2 has no null values */
/* Implementation uses binary search which I do not like */
/* Usually used when T1 is large and T2 is small */

//---------------------------------------------------------------
// START FUNC DECL
int 
is_a_in_b(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld,
	  char *src_f2, /* fld_to_fetch_from_t2 */
	  char *dst_f1 /* fld_to_place_in_t1 */
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *src_f2_X = NULL; size_t src_f2_nX = 0;
  char *dst_f1_X = NULL; size_t dst_f1_nX = 0;
  char *cfX = NULL; size_t cf_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_src_f2_X = NULL; size_t nn_src_f2_nX = 0;
  FLD_REC_TYPE f1_rec, f2_rec, src_f2_rec, dst_f1_rec, cfld_rec;
  FLD_REC_TYPE nn_f1_rec, nn_f2_rec, nn_src_f2_rec, nn_dst_f1_rec;
  TBL_REC_TYPE t1_rec, t2_rec;
  long long nR1, nR2;
  int t1_id, t2_id; 
  int f1_id, nn_f1_id; 
  int f2_id, nn_f2_id; 
  int src_f2_id = INT_MIN, nn_src_f2_id = INT_MIN;
  int dst_f1_id = INT_MIN, nn_dst_f1_id = INT_MIN; 
  int cfld_id = INT_MIN;
  bool b_write_dst;
  int minval_I4 = INT_MAX, maxval_I4 = INT_MIN; 
  long long minval_I8 = LLONG_MAX, maxval_I8 = LLONG_MIN;
  int *f1_I4 = NULL, *f2_I4 = NULL, *src_f2_I4 = NULL;
  long long *f1_I8 = NULL, *f2_I8 = NULL, *src_f2_I8 = NULL;
  int fldsz; 
  int ddir_id = -1, fileno = -1;
  int dst_ddir_id = -1, dst_fileno = -1;
#define BUFSZ 16
  char buf[BUFSZ];
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) {  
    /* condition field NOT provided */
    if ( ( src_f2 == NULL ) || ( *src_f2 == '\0' ) ) { go_BYE(-1); }
    if ( ( dst_f1 == NULL ) || ( *dst_f1 == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(f1, dst_f1) == 0 ) { go_BYE(-1); }
  }
  else {
    if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
    if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { go_BYE(-1); }
    if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR; if ( nR1 <= 0 ) { go_BYE(-1); }
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  if ( f1_id < 0 ) { 
    fprintf(stderr, "Field [%s] in Table [%s] missing\n", f1, t1);
    go_BYE(-1); 
  }
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  f1_I4 = (int *)f1_X;
  f1_I8 = (long long *)f1_X;

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  f2_I4 = (int *)f2_X;
  f2_I8 = (long long *)f2_X;
  status = f_to_s(t2, f2, "is_sorted", buf, BUFSZ); cBYE(status);
  if ( strcmp(buf, "ascending") != 0 ) { go_BYE(-1); }

  //--------------------------------------------------------
  if (f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { 
    status = is_fld(NULL, t2_id, src_f2, &src_f2_id, &src_f2_rec, 
		    &nn_src_f2_id, &nn_src_f2_rec);
    status = get_data(src_f2_rec, &src_f2_X, &src_f2_nX, false); 
    cBYE(status);
    src_f2_I4 = (int *)src_f2_X;
    src_f2_I8 = (long long *)src_f2_X;
    if ( nn_src_f2_id >= 0 ) { 
      status = get_data(nn_src_f2_rec, &nn_src_f2_X, &nn_src_f2_nX, false); 
      cBYE(status);
    }
  }
  /*------------------------------------------------------------*/
  // There can be null values in f1 but not in f2
  if ( nn_f1_id >= 0 ) { 
    chk_range(nn_f1_id, 0, g_n_fld);
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); 
    cBYE(status);
  }
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  /* Determine whether a destination value needs to be written in T1 */
  b_write_dst = false;
  if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) {
    b_write_dst = true;
    if ( f1_rec.fldtype != src_f2_rec.fldtype ) { go_BYE(-1); }
  }
  /*------------------------------------------------------------*/
  /* Make space for output condition field */
  status = mk_temp_file((nR1 *sizeof(char)), &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &cfX, &cf_nX, 1); cBYE(status);
  /* Make space for output value field */
  if ( b_write_dst ) { 
    status = get_fld_sz(src_f2_rec.fldtype, &fldsz); cBYE(status);
    status = mk_temp_file((nR1 * fldsz), &dst_ddir_id, &dst_fileno); cBYE(status);
    status = q_mmap(dst_ddir_id, dst_fileno, &dst_f1_X, &dst_f1_nX, 1); // writing 
  }
  //--------------------------------------------------------
  /* We find the smallest and largest values of f2 which allow us to
   * skip the binary search over the values of f2 */
  switch ( f2_rec.fldtype ) { 
  case I4 : 
    minval_I4 = f2_I4[0];
    maxval_I4 = f2_I4[nR2-1];
    break;
  case I8 : 
    minval_I8 = f2_I8[0];
    maxval_I8 = f2_I8[nR2-1];
    break;
  case I1 : case I2 : case F4 : case F8 : 
    go_BYE(-1);
    break;
  default : 
    go_BYE(-1);
    break;
  }
  /*------------------------------------------------------------------*/
  /* prev_defined tells us whether prev_I4val1 or prev_I8val1 are defined */

  long long block_size; int nT;
  status = partition(nR1, 1024, -1, &block_size, &nT); cBYE(status);
  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR1; }

    // These are kept local to avoid sharing issues
    int *dst_f1_I4 = (int *)dst_f1_X;
    long long *dst_f1_I8 = (long long *)dst_f1_X;
    int f1_val_I4 = INT_MIN, prev_f1_val_I4 = INT_MIN, src_f2_val_I4;
    long long f1_val_I8 = LLONG_MIN, prev_f1_val_I8 = LLONG_MIN, src_f2_val_I8;
    long long prev_pos = -1; // undefined value 
    bool prev_defined = false; 
    //-----------------------------------
    for  ( long long i1 = lb; i1 < ub; i1++ ) {
      char nn_src_f2_val = FALSE;
      long long pos;
      bool is_found = false;
      char *l_cfX = cfX; // make a local copy
      /* If f1 is null, then cfld and dst_f1 must also be null */
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
	src_f2_val_I4 = 0;
        nn_src_f2_val = FALSE;
	l_cfX[i1] = FALSE;
	if ( b_write_dst ) { 
          switch ( f1_rec.fldtype ) { 
	    case I4 : dst_f1_I4[i1] = 0; break;
	    case I8 : dst_f1_I8[i1] = 0; break;
	    default : status = -1; break;
	  }
	}
	continue;
      }
      /*----------------------------------------------------------------*/
      nn_src_f2_val = FALSE;
      switch ( f1_rec.fldtype ) { 
      case I4 : 
	f1_val_I4 = f1_I4[i1];
	if ( ( f1_val_I4 < minval_I4 ) || ( f1_val_I4 > maxval_I4 ) ) {
	  pos = -1;
	}
	else {
	  if ( ( prev_defined ) && ( f1_val_I4 == prev_f1_val_I4 ) ) { 
	    if ( prev_pos >= 0 ) { 
	      pos = prev_pos;
	    }
	    else {
	      status = bin_search_I4(f2_I4, nR2, f1_val_I4, &pos, "");
	    }
	  }
	  else {
	    status = bin_search_I4(f2_I4, nR2, f1_val_I4, &pos, "");
	  }
	}
	if ( pos >= 0 ) { is_found = true; } else { is_found = false; }
	if ( b_write_dst ) { 
	  if ( is_found == true ) {
	    src_f2_val_I4 = src_f2_I4[pos];
	    if ( nn_src_f2_id >= 0 ) { 
	      nn_src_f2_val = nn_src_f2_X[pos];
	    }
	    else {
	      nn_src_f2_val = TRUE;
	    }
	  }
	  else {
	    src_f2_val_I4 = 0;
	    nn_src_f2_val = FALSE;
	  }
	  dst_f1_I4[i1] = src_f2_val_I4;
	}
	else {
	  if ( pos >= 0 ) { nn_src_f2_val = TRUE; } 
	}
	break;
      case I8 : 
	f1_val_I8 = f1_I8[i1];
	if ( ( f1_val_I8 < minval_I8 ) || ( f1_val_I8 > maxval_I8 ) ) {
	  pos = -1;
	}
	else {
	  if ( ( prev_defined ) && ( f1_val_I8 == prev_f1_val_I8 ) ) { 
	    if ( prev_pos >= 0 ) { 
	      pos = prev_pos;
	    }
	    else {
	      status = bin_search_I8(f2_I8, nR2, f1_val_I8, &pos, "");
	    }
	  }
	  else {
	    status = bin_search_I8(f2_I8, nR2, f1_val_I8, &pos, "");
	  }
	}
	if ( pos >= 0 ) { is_found = true; } else { is_found = false; }
	if ( b_write_dst ) {
	  if ( is_found == true ) { 
	    src_f2_val_I8 = src_f2_I8[pos];
	    if ( nn_src_f2_id >= 0 ) { 
	      nn_src_f2_val = nn_src_f2_X[pos];
	    }
	    else {
	      nn_src_f2_val = TRUE;
	    }
	  }
	  else {
	    src_f2_val_I8 = 0;
	    nn_src_f2_val = FALSE;
	  }
	  dst_f1_I8[i1] = src_f2_val_I8;
	}
	else {
	  if ( pos >= 0 ) { nn_src_f2_val = TRUE; } 
	}
	break;
      default :
	if ( status == 0 ) { WHEREAMI; } status = -1; 
	break;
      }
      prev_f1_val_I4 = f1_val_I4;
      prev_f1_val_I8 = f1_val_I8;
      prev_defined = true;
      if ( is_found == true ) { 
        l_cfX[i1] = nn_src_f2_val; // TODO P2: Check this code 
      }
      else {
	l_cfX[i1] = FALSE;
      }
    }
  }
  cBYE(status);

  // Add output field to meta data 
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    zero_fld_rec(&cfld_rec); cfld_rec.fldtype = I1;
    status = add_fld(t1_id, cfld, ddir_id, fileno, &cfld_id, &cfld_rec);
    cBYE(status);
  }
  else {
    switch ( src_f2_rec.fldtype ) { 
    case I1 : case I2 : case F4 : case F8 : 
      go_BYE(-1);
      break;
    case I4 : 
      zero_fld_rec(&dst_f1_rec); dst_f1_rec.fldtype = I4;
      status = add_fld(t1_id, dst_f1, dst_ddir_id, dst_fileno, &dst_f1_id, &dst_f1_rec);
      cBYE(status);
      break;
    case I8 : 
      zero_fld_rec(&dst_f1_rec); dst_f1_rec.fldtype = I8;
      status = add_fld(t1_id, dst_f1, dst_ddir_id, dst_fileno, &dst_f1_id, &dst_f1_rec);
      cBYE(status);
      break;
    default : 
      go_BYE(-1);
      break;
    }
    /* TODO: P2: Add nn field only if necessary */
    zero_fld_rec(&nn_dst_f1_rec); nn_dst_f1_rec.fldtype = I1;
    status = add_aux_fld(NULL, t1_id, NULL, dst_f1_id, ddir_id, fileno, 
			 nn, &nn_dst_f1_id,  &nn_dst_f1_rec);
    cBYE(status);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(src_f2_X, src_f2_nX);
  rs_munmap(dst_f1_X, dst_f1_nX);
  rs_munmap(cfX, cf_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(nn_src_f2_X, nn_src_f2_nX);
  return(status);
}
