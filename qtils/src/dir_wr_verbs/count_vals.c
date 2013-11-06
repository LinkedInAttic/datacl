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
#include "aux_meta.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "set_meta.h"
#include "ext_f_to_s.h"
#include "count_vals.h"

#include "meta_globals.h"

#include "uniq_I4.h"
#include "uniq_I8.h"
#include "uniq_cnt_I4.h"
#include "uniq_cnt_I8.h"
#include "uniq_alldef_I4.h"
#include "uniq_alldef_I8.h"
#include "uniq_cnt_alldef_I4.h"
#include "uniq_cnt_alldef_I8.h"

extern char *g_data_dir;
extern char *g_cwd;

//---------------------------------------------------------------
// START FUNC DECL
int 
count_vals(
	   char *src_tbl,
	   char *src_fld,
	   char *in_cnt_fld,
	   char *dst_tbl,
	   char *dst_fld,
	   char *out_cnt_fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *in_cnt_fld_X = NULL; size_t in_cnt_fld_nX = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, in_cnt_fld_rec;
  FLD_REC_TYPE nn_in_cnt_fld_rec, nn_src_fld_rec;
  FLD_REC_TYPE dst_fld_rec, cnt_fld_rec;
  long long src_nR; 
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int dst_fld_id = INT_MIN;
  int in_cnt_fld_id = INT_MIN, nn_in_cnt_fld_id = INT_MIN;
  int cnt_fld_id = INT_MIN;
  long long dst_nR, n_out;
#define BUFLEN 32
  char buffer[BUFLEN];
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *val_X = NULL; size_t val_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;
  char val_opfile[MAX_LEN_FILE_NAME+1];
  char cnt_opfile[MAX_LEN_FILE_NAME+1];
  int fldsz = INT_MAX, val_ddir_id = INT_MAX,  cnt_ddir_id = INT_MAX;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(buffer, BUFLEN);
  zero_string(val_opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(cnt_opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); 
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, false); cBYE(status);
  if ( nn_src_fld_id >= 0 ) { 
    status = get_data(nn_src_fld_rec, &nn_src_fld_X, &nn_src_fld_nX, false); 
    cBYE(status);
  }
  // Get the count field if specified. It cannot have null values
  if ( *in_cnt_fld != '\0' ) { 
    status = is_fld(NULL, src_tbl_id, in_cnt_fld, &in_cnt_fld_id, &in_cnt_fld_rec,
		    &nn_in_cnt_fld_id, &nn_in_cnt_fld_rec);
    status = get_data(in_cnt_fld_rec, &in_cnt_fld_X, &in_cnt_fld_nX, 0); cBYE(status);
    if ( nn_in_cnt_fld_id >= 0 ) { go_BYE(-1); }
    if ( in_cnt_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  }
  /* Make sure src_fld field is sorted ascending */
  zero_string(buffer, BUFLEN);
  status = ext_f_to_s(src_tbl, src_fld, "is_sorted", buffer, BUFLEN); cBYE(status);
  if ( strcmp(buffer, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  // Create output space. May need to be truncated later on
  // TODO P2: Need to have some idea of number of unique values so that
  // we can be sure that cnt is < INT_MAX
  status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(val_opfile, (src_nR * fldsz), &val_ddir_id); cBYE(status);
  status = q_mmap(val_ddir_id, val_opfile, &val_X, &val_nX, true); cBYE(status);

  status = get_fld_sz(I4, &fldsz); cBYE(status);
  status = mk_temp_file(cnt_opfile, (src_nR * fldsz), &cnt_ddir_id); cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_opfile, &cnt_X, &cnt_nX, true); cBYE(status);
  //------------------------------------------------------
  n_out = src_nR * sizeof(int); // upper bound 
  int *in_cnt_I4 = (int *)in_cnt_fld_X;
  int *op_cnt_I4 = (int *)cnt_X;

  if ( src_fld_rec.fldtype == I4 ) { 
    if ( nn_src_fld_X == NULL ) { 
      if ( *in_cnt_fld == '\0' ) { 
	uniq_alldef_I4((int *)src_fld_X, src_nR, (int *)val_X, 
			&dst_nR, op_cnt_I4); 
      }
      else {
	uniq_cnt_alldef_I4((int *)src_fld_X, in_cnt_I4, 
			    src_nR, (int *)val_X, &dst_nR, op_cnt_I4);
      }
    }
    else {
      if ( *in_cnt_fld == '\0' ) { 
        uniq_I4((int *)src_fld_X, src_nR, nn_src_fld_X,
	       (int *)val_X, &dst_nR, op_cnt_I4);
      }
      else {
        uniq_cnt_I4((int *)src_fld_X, in_cnt_I4, src_nR, nn_src_fld_X,
	       (int *)val_X, &dst_nR, op_cnt_I4);
      }
    }
  }
  else if ( src_fld_rec.fldtype == I8 ) { 
    if ( nn_src_fld_X == NULL ) { 
      if ( *in_cnt_fld == '\0' ) { 
	uniq_alldef_I8((long long *)src_fld_X, src_nR, 
			     (long long *)val_X, &dst_nR, op_cnt_I4);
      }
      else {
	if ( in_cnt_fld_rec.fldtype == I8 ) { 
	  uniq_cnt_alldef_I8((long long *)src_fld_X, in_cnt_I4, src_nR, 
	      (long long *)val_X, &dst_nR, op_cnt_I4);
	}
      }
    }
    else {
      if ( *in_cnt_fld == '\0' ) { 
        uniq_I8((long long *)src_fld_X, src_nR, nn_src_fld_X, 
		      (long long *)val_X, &dst_nR, op_cnt_I4);
      }
      else {
        uniq_cnt_I8((long long *)src_fld_X, in_cnt_I4, src_nR, nn_src_fld_X,
	       (long long *)val_X, &dst_nR, op_cnt_I4);
      }
    }
  }
  else { go_BYE(-1); }
  if ( dst_nR == 0 ) { goto BYE; }
  //-----------------------------------------------------------
  status = del_tbl(dst_tbl, -1); cBYE(status);
  sprintf(buffer, "%lld", dst_nR);
  status = add_tbl(dst_tbl, buffer, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  //-----------------------------------------------------------
  // Truncate output files to correct sise 
  if ( dst_nR < src_nR ) { 
    long long wridx = 0; // write index
    // rdidx = read index 
    int *I4cnt = (int *)cnt_X;
    if ( src_fld_rec.fldtype == I4 ) { 
      for ( long long rdidx = 0; rdidx < src_nR; rdidx++ ) {
	int *I4val = (int *)val_X;
	if ( I4cnt[rdidx] > 0 ) {
	  I4val[wridx] = I4val[rdidx];
	  I4cnt[wridx] = I4cnt[rdidx];
	  wridx++;
	}
      }
    }
    else if ( src_fld_rec.fldtype == I8 ) { 
      for ( long long rdidx = 0; rdidx < src_nR; rdidx++ ) {
	int *I8val = (int *)val_X;
	if ( I4cnt[rdidx] > 0 ) {
	  I8val[wridx] = I8val[rdidx];
	  I4cnt[wridx] = I4cnt[rdidx];
	  wridx++;
	}
      }
    }
    else { 
      go_BYE(-1); 
    }
    if ( wridx != dst_nR ) { go_BYE(-1); }
    rs_munmap(val_X, val_nX);
    status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);
    status = q_trunc(val_ddir_id, val_opfile, (dst_nR * fldsz)); cBYE(status);
  
    rs_munmap(cnt_X, cnt_nX);
    status = get_fld_sz(I4, &fldsz); cBYE(status);
    status = q_trunc(cnt_ddir_id, cnt_opfile, (dst_nR * fldsz)); cBYE(status);
  }
  // Add value field to dst table
  status = add_fld(dst_tbl_id, dst_fld, val_ddir_id, val_opfile, 
      src_fld_rec.fldtype, -1, &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  // Since src_fld is sorted ascending, so is dst_fld 
  status = int_set_meta(dst_tbl_id, dst_fld_id, "sort_type", "ascending");
  // Add count field to dst table 
  status = add_fld(dst_tbl_id, out_cnt_fld, cnt_ddir_id, cnt_opfile, I4, 
		   -1, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(in_cnt_fld_X, in_cnt_fld_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);

  rs_munmap(cnt_X, cnt_nX);
  rs_munmap(val_X, val_nX);
  return(status);
}
