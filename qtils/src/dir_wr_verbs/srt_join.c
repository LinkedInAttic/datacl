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
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "open_temp_file.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "del_fld.h"
#include "ext_f_to_s.h"
#include "meta_globals.h"

#include "core_srt_join_I_I_I.h"
#include "core_srt_join_I_I_L.h"
#include "core_srt_join_I_L_I.h"
#include "core_srt_join_I_L_L.h"
#include "core_srt_join_L_I_I.h"
#include "core_srt_join_L_I_L.h"
#include "core_srt_join_L_L_I.h"
#include "core_srt_join_L_L_L.h"

/* srt_join is used when the source link fields and destination fields are
 * sorted and have no null values */
//---------------------------------------------------------------


// START FUNC DECL
int 
srt_join(
	 char *src_tbl,
	 char *src_lnk,
	 char *src_val,
	 char *dst_tbl,
	 char *dst_lnk,
	 char *dst_val,
	 char *op
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *src_val_X = NULL; size_t src_val_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;

  char *nn_dst_val_X = NULL; size_t nn_dst_val_nX = 0;
  char *dst_val_X = NULL; size_t dst_val_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_val_rec, nn_src_val_rec;
  FLD_REC_TYPE src_lnk_rec, nn_src_lnk_rec;
  FLD_REC_TYPE dst_lnk_rec, nn_dst_lnk_rec;
  FLD_REC_TYPE dst_val_rec, nn_dst_val_rec;
  long long src_nR = INT_MIN, dst_nR = INT_MIN;
  int ijoin_op; bool is_any_null = false;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_lnk_id = INT_MIN, nn_src_lnk_id = INT_MIN;
  int dst_lnk_id = INT_MIN, nn_dst_lnk_id = INT_MIN;
  int src_val_id = INT_MIN, nn_src_val_id = INT_MIN;
  int dst_val_id = INT_MIN, nn_dst_val_id = INT_MIN;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
#define RSLT_LEN 32
  char str_rslt[RSLT_LEN];
  //----------------------------------------------------------------
  zero_string(str_rslt, RSLT_LEN);
  status = mk_mjoin_op(op, &ijoin_op);
  cBYE(status);
  //--------------------------------------------------------
  // We support a special operator called cnt which can be used only
  // when src_val is null. 
  if ( ( src_val == NULL ) || ( *src_val == '\0' ) ) {
    if ( ( strcmp(op, "cnt") != 0 ) ) { go_BYE(-1); }
    zero_fld_rec(&src_val_rec);
  }
  else {
    if ( ( strcmp(op, "cnt") == 0 ) ) { go_BYE(-1); }
  }
  //----------------------------------------------------------------
  // Get meta-data for all necessary fields 
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_lnk, &src_lnk_id, &src_lnk_rec,
      &nn_src_lnk_id, &nn_src_lnk_rec); cBYE(status);
  chk_range(src_lnk_id, 0, g_n_fld);

  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
  status = is_fld(NULL, src_tbl_id, src_val,  &src_val_id, &src_val_rec,
      &nn_src_val_id, &nn_src_val_rec); cBYE(status);
  chk_range(src_val_id, 0, g_n_fld);
  }

  status = is_fld(NULL, dst_tbl_id, dst_lnk,  &dst_lnk_id, &dst_lnk_rec,
      &nn_dst_lnk_id, &nn_dst_lnk_rec); cBYE(status);
  chk_range(dst_lnk_id, 0, g_n_fld);

  //----------------------------------------------------------------
  /* Make sure link fields are sorted ascending */
  status = ext_f_to_s(src_tbl, src_lnk, "is_sorted", str_rslt, RSLT_LEN);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_lnk, src_tbl);
    go_BYE(-1);
  }
  status = ext_f_to_s(dst_tbl, dst_lnk, "is_sorted", str_rslt, RSLT_LEN);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n", 
	    dst_lnk, dst_tbl);
    go_BYE(-1);
  }
  //----------------------------------------------------------------
  // Get pointer access to all necessary fields
  status = get_data(src_lnk_rec, &src_lnk_X, &src_lnk_nX, false); cBYE(status);
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = get_data(src_val_rec, &src_val_X, &src_val_nX, false); cBYE(status);
  }
  status = get_data(dst_lnk_rec, &dst_lnk_X, &dst_lnk_nX, false); cBYE(status);

  // Create output data files
  if ( ijoin_op == MJOIN_OP_CNT ) {
    if ( src_nR < INT_MAX ) { 
      src_val_rec.fldtype = I4;
    }
    else {
      src_val_rec.fldtype = I8;
    }
  }
  int fldsz = INT_MAX, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;

  status = get_fld_sz(src_val_rec.fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(opfile, (fldsz * dst_nR), &ddir_id);

  status = get_fld_sz(I1, &fldsz); cBYE(status);
  status = mk_temp_file(nn_opfile,  (fldsz * dst_nR), &nn_ddir_id);

  status = q_mmap(ddir_id, opfile, &dst_val_X, &dst_val_nX, 1); cBYE(status);
  status = q_mmap(nn_ddir_id, nn_opfile, &nn_dst_val_X, &nn_dst_val_nX, 1); cBYE(status);
  //--------------------------------------------
  long long block_size; int nT;
  status = partition(dst_nR, 1024, -1, &block_size, &nT); cBYE(status);

  for ( int tid = 0; tid < nT; tid++ ) {

    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = dst_nR; }

    int *I4_src_lnk = (int *)src_lnk_X;
    int *I4_src_val = (int *)src_val_X;
    int *I4_dst_lnk = (int *)dst_lnk_X;
    int *I4_dst_val = (int *)dst_val_X;

    long long *I8_src_lnk = (long long *)src_lnk_X;
    long long *I8_src_val = (long long *)src_val_X;
    long long *I8_dst_lnk = (long long *)dst_lnk_X;
    long long *I8_dst_val = (long long *)dst_val_X;

    char *dst_nn    = nn_dst_val_X;

    // Core join 
    if ( ( src_lnk_rec.fldtype  == I4 ) && 
	 ( src_val_rec.fldtype  == I4 ) && 
	 ( dst_lnk_rec.fldtype  == I4 ) ) {
      status = core_srt_join_I_I_I(
	  I4_src_lnk, I4_src_val, src_nR, I4_dst_lnk, I4_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	 ( src_val_rec.fldtype       == I4 ) && 
	 ( dst_lnk_rec.fldtype       == I8 ) ) {
      status = core_srt_join_I_I_L(
	  I4_src_lnk, I4_src_val, src_nR, I8_dst_lnk, I4_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	 ( src_val_rec.fldtype       == I8 ) && 
	 ( dst_lnk_rec.fldtype       == I4 ) ) {
      status = core_srt_join_I_L_I(
	  I4_src_lnk, I8_src_val, src_nR, I4_dst_lnk, I8_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	 ( src_val_rec.fldtype       == I8 ) && 
	 ( dst_lnk_rec.fldtype       == I8 ) ) {
      status = core_srt_join_I_L_L(
	  I4_src_lnk, I8_src_val, src_nR, I8_dst_lnk, I8_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	 ( src_val_rec.fldtype       == I4 ) && 
	 ( dst_lnk_rec.fldtype       == I4 ) ) {
      status = core_srt_join_L_I_I(
	  I8_src_lnk, I4_src_val, src_nR, I4_dst_lnk, I4_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	 ( src_val_rec.fldtype       == I4 ) && 
	 ( dst_lnk_rec.fldtype       == I8 ) ) {
      status = core_srt_join_L_I_L(
	  I8_src_lnk, I4_src_val, src_nR, I8_dst_lnk, I4_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	 ( src_val_rec.fldtype       == I8 ) && 
	 ( dst_lnk_rec.fldtype       == I4 ) ) {
      status = core_srt_join_L_L_I(
	  I8_src_lnk, I8_src_val, src_nR, I4_dst_lnk, I8_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	 ( src_val_rec.fldtype       == I8 ) && 
	 ( dst_lnk_rec.fldtype       == I8 ) ) {
      status = core_srt_join_L_L_L(
	  I8_src_lnk, I8_src_val, src_nR, I8_dst_lnk, I8_dst_val, dst_nn,
	  lb, ub, ijoin_op, &is_any_null);
    }
    else { go_BYE(-1); }
  }
  if ( is_any_null ) { is_any_null = true; }
  //--------------------------------------------------------
  // Add output field to meta data 
  status = add_fld(dst_tbl_id, dst_val, ddir_id, opfile, src_val_rec.fldtype, 
	-1, &dst_val_id, &dst_val_rec);
  cBYE(status);
  if ( is_any_null == true ) {
    status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_val_id, nn_ddir_id,
	nn_opfile, "nn", &nn_dst_val_id, &nn_dst_val_rec);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_val_X, dst_val_nX);
  rs_munmap(nn_dst_val_X, nn_dst_val_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);
  return(status);
}
