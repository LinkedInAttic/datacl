/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
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
#include "open_temp_file.h"
#include "mix.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
count_ht(
	 char *src_tbl,
	 char *fld,
	 char *dst_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;    size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  TBL_REC_TYPE src_tbl_rec; int src_tbl_id; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id; 
  FLD_REC_TYPE fld_rec;     int fld_id; 
  FLD_REC_TYPE nn_fld_rec;  int nn_fld_id; 
  FLD_REC_TYPE val_fld_rec;  int val_fld_id; 
  FLD_REC_TYPE cnt_fld_rec;  int cnt_fld_id; 
  long long src_nR, dst_nR;
#define MAX_LEN 32
  char op[MAX_LEN];
  char str_dst_nR[MAX_LEN];
  char valfile[MAX_LEN_FILE_NAME+1];
  char cntfile[MAX_LEN_FILE_NAME+1];
  char *val_X = NULL; size_t n_val_X = 0;
  char *cnt_X = NULL; size_t n_cnt_X = 0;
  // for final dumping of results
  char *I1ptr = NULL; 
  short *I2ptr = NULL; 
  int *I4ptr = NULL; int I4val; 
  long long *I8ptr = NULL; long long I8val;
  //-- for pthreads
  HT_I8_TYPE *ht = NULL;
  int sz_ht;    // size of hash table 
  int n_ht = 0;  // number of occupied entries in hash table 
  unsigned long long ulval;
  int ht_loc, pos_ht; bool space_found, key_exists; 
  long long num_probes = 0; long long max_count = 0;
  int val_ddir_id = INT_MAX, cnt_ddir_id = INT_MAX;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  zero_string(op, MAX_LEN);
  zero_string(str_dst_nR, MAX_LEN);
  zero_string(valfile, MAX_LEN_FILE_NAME+1);
  zero_string(cntfile, MAX_LEN_FILE_NAME+1);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  status = is_fld(NULL, src_tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);

  status = get_data(fld_rec, &X, &nX, 0); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, 0); cBYE(status);
    cBYE(status);
  }
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  sz_ht = prime_geq(65536);
  ht = (HT_I8_TYPE *)malloc(sz_ht * sizeof(HT_I8_TYPE));
  return_if_malloc_failed(ht);
  for ( int i = 0; i < sz_ht; i++ ) { 
    ht[i].key = 0;
    ht[i].cnt = 0;
  } 
  I1ptr = (char *)X;
  I2ptr = (short *)X;
  I4ptr = (int *)X;
  I8ptr = (long long *)X;

  for ( long long i = 0; i < src_nR; i++ ) {
    if ( ( nn_X != NULL ) && ( nn_X[i] == FALSE ) ) {
      continue;
    }
    switch ( fld_rec.fldtype ) { 
    case I4 : 
      I4val = I4ptr[i]; ulval = (unsigned long long)I4val;
      break;
    case I8 : 
      I8val = I8ptr[i]; ulval = (unsigned long long)I8val;
      break;
    default : 
      go_BYE(-1);
      break;
    }
    ht_loc = mix_I8(ulval) % sz_ht;
    pos_ht = -1;
    // Determine whether you have seen this guy before or not
    // If not, then you also need to find the position to put him
    space_found = false; 
    for ( int i = ht_loc; i < sz_ht; i++ ) { 
      num_probes++;
      if ( ht[i].key == ulval ) { 
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
	if ( ht[i].key == ulval ) { 
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
    if ( space_found == false ) { go_BYE(-1); }
    if ( pos_ht < 0 ) { go_BYE(-1); }
    if ( key_exists ) {
      if ( ht[pos_ht].key != ulval ) { 
	go_BYE(-1);
      }
      if ( pos_ht == 1 ) {
	go_BYE(-1);
      }
      if ( ht[pos_ht].cnt == 0 ) {
        ht[pos_ht].cnt = 1;
        ht[pos_ht].key = ulval;
	n_ht++;
      }
      ht[pos_ht].cnt = ht[pos_ht].cnt + 1;
      if ( ht[pos_ht].cnt > max_count ) { 
	max_count = ht[pos_ht].cnt;
      }
    }
    else {
      ht[pos_ht].cnt = 1;
      ht[pos_ht].key = ulval;
      n_ht++;
    }
    if ( n_ht > ( sz_ht >> 1 ) ) {
      fprintf(stderr, "TO BE IMPLEMENTED\n");
      go_BYE(-1);
    }
  }


  dst_nR = n_ht;
  sprintf(str_dst_nR, "%lld", dst_nR);
  //-------------------------------------------------------
  status = del_tbl(dst_tbl, -1); cBYE(status);
  status = add_tbl(dst_tbl, str_dst_nR, &dst_tbl_id, &dst_tbl_rec); 
  cBYE(status);
  int fldsz; status = get_fld_sz(fld_rec.fldtype, &fldsz); cBYE(status);

  status = mk_temp_file(valfile, fldsz * dst_nR, &val_ddir_id); cBYE(status);
  status = q_mmap(val_ddir_id, valfile, &val_X, &n_val_X, 1); cBYE(status);
  I4ptr = (int *)val_X;
  I8ptr = (long long *)val_X;
  int ctr = 0;
  for ( int i = 0; i < sz_ht; i++ ) {
    if ( ht[i].cnt == 0 ) {  continue; }
    ctr++;
    switch ( fld_rec.fldtype ) { 
    case I4 : I4val = ht[i].key; *I4ptr++ = I4val; break;
    case I8 : I8val = ht[i].key; *I8ptr++ = I8val; break;
    default : go_BYE(-1); break;
    }
  }
  if ( ctr != n_ht ) { 
    go_BYE(-1); 
  }
  rs_munmap(val_X, n_val_X);
  status = add_fld(dst_tbl_id, "value", val_ddir_id, valfile, fld_rec.fldtype, 
		   -1, &val_fld_id, &val_fld_rec);

  if ( max_count >= INT_MAX ) { fprintf(stderr, "TODO\n"); go_BYE(-1); }

  status = mk_temp_file(cntfile, (sizeof(int) * dst_nR), &cnt_ddir_id); cBYE(status);
  status = q_mmap(cnt_ddir_id, cntfile, &cnt_X, &n_cnt_X, 1); cBYE(status);
  I4ptr = (int *)cnt_X;
  I8ptr = (long long *)cnt_X;
  ctr = 0;
  for ( int i = 0; i < sz_ht; i++ ) {
    if ( ht[i].cnt == 0 ) { continue; }
    ctr++;
    I4val = ht[i].cnt; *I4ptr++ = I4val; 
  }
  if ( ctr != n_ht ) { go_BYE(-1); }
  rs_munmap(cnt_X, n_cnt_X);
  status = add_fld(dst_tbl_id, "count", cnt_ddir_id, cntfile, I4, -1, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
  // fprintf(stderr, "Average num probes = %lf \n", (double)num_probes/(double)src_nR);

 BYE:
  free_if_non_null(ht);
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(val_X, n_val_X);
  rs_munmap(cnt_X, n_cnt_X);
  return(status);
}
