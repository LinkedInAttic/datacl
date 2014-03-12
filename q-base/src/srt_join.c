#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "del_fld.h"
#include "f_to_s.h"
#include "meta_globals.h"

#include "core_srt_join_I4_I4_I4.h"
#include "core_srt_join_I4_I4_I8.h"
#include "core_srt_join_I4_I8_I4.h"
#include "core_srt_join_I4_I8_I8.h"
#include "core_srt_join_I8_I4_I4.h"
#include "core_srt_join_I8_I4_I8.h"
#include "core_srt_join_I8_I8_I4.h"
#include "core_srt_join_I8_I8_I8.h"

extern int g_num_cores;

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
  
#define RSLT_LEN 32
  char str_rslt[RSLT_LEN];
  //----------------------------------------------------------------
  zero_string(str_rslt, RSLT_LEN);
  status = mk_join_op(op, &ijoin_op); cBYE(status);
  zero_fld_rec(&src_val_rec); zero_fld_rec(&nn_src_val_rec);
  zero_fld_rec(&src_lnk_rec); zero_fld_rec(&nn_src_lnk_rec);
  zero_fld_rec(&dst_lnk_rec); zero_fld_rec(&nn_dst_lnk_rec);
  zero_fld_rec(&dst_val_rec); zero_fld_rec(&nn_dst_val_rec);
  //--------------------------------------------------------
  // We support special operators, cnt, minidx, maxidx which can be used 
  // only when src_val is null. 
  if ( ( src_val == NULL ) || ( *src_val == '\0' ) ) {
    if ( ( strcmp(op, "cnt") != 0 ) && 
	 ( strcmp(op, "minidx") != 0 ) && ( strcmp(op, "maxidx") != 0 ) ) {
      go_BYE(-1); 
    }
  }
  else {
    if ( ( strcmp(op, "cnt") == 0 ) ||
	 ( strcmp(op, "minidx") == 0 ) || ( strcmp(op, "maxidx") == 0 ) ) {
      go_BYE(-1); 
    }
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
  if ( nn_src_lnk_id >= 0 ) { go_BYE(-1); } // no nn field for src_lnk

  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, src_val,  &src_val_id, &src_val_rec,
		    &nn_src_val_id, &nn_src_val_rec); cBYE(status);
    chk_range(src_val_id, 0, g_n_fld);
    if ( nn_src_val_id >= 0 ) { go_BYE(-1); } // no nn field for src_val
  }

  status = is_fld(NULL, dst_tbl_id, dst_lnk,  &dst_lnk_id, &dst_lnk_rec,
		  &nn_dst_lnk_id, &nn_dst_lnk_rec); cBYE(status);
  chk_range(dst_lnk_id, 0, g_n_fld);
  if ( nn_dst_lnk_id >= 0 ) { go_BYE(-1); } // no nn field for dst_lnk
  //----------------------------------------------------------------
  /* Make sure link fields are sorted ascending */
  status = f_to_s(src_tbl, src_lnk, "is_sorted", str_rslt, RSLT_LEN);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_lnk, src_tbl);
    go_BYE(-1);
  }
  status = f_to_s(dst_tbl, dst_lnk, "is_sorted", str_rslt, RSLT_LEN);
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

  // Special case when there is no source field 
  switch ( ijoin_op ) { 
  case join_exists : 
    dst_val_rec.fldtype = I1;
    break;
  case join_cnt : 
  case join_minidx : 
  case join_maxidx : 
    dst_val_rec.fldtype = I8;
    /* TODO: P3: When src_nR < INT_MAX, one can use I4 */
    break;
  default : 
    dst_val_rec.fldtype = src_val_rec.fldtype;
    
    break;
  }
  // Create output data files
  int fldsz = INT_MAX; size_t filesz = 0;
  int ddir_id    = -1, fileno    = -1;
  int nn_ddir_id = -1, nn_fileno = -1; 

  status = get_fld_sz(dst_val_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * dst_nR;
  status = mk_temp_file(filesz, &ddir_id, &fileno);
  status = q_mmap(ddir_id, fileno, &dst_val_X, &dst_val_nX, 1); cBYE(status);

  // No nn field needed for "cnt" and "sum" and "minidx" and "maxidx"
  // When there is no match, 
  // (1) cnt and sum get set to 0
  // (1) minidx and maxidx get set to -1
  switch ( ijoin_op ) { 
  case join_cnt : 
  case join_sum : 
  case join_minidx : 
  case join_maxidx : 
  case join_exists : 
    break;
  default : 
    status = get_fld_sz(I1, &fldsz); cBYE(status);
    filesz = fldsz * dst_nR;
    status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_dst_val_X, &nn_dst_val_nX, 1); 
    cBYE(status);
  }
  //--------------------------------------------
  int nT; long long block_size;
  if ( dst_nR < (g_num_cores * 4096) ) {
    nT = 1; block_size = dst_nR;
  }
  else {
    nT = g_num_cores * 4;
    block_size = dst_nR / nT;
  }

#pragma omp parallel for 
  for ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }

    bool t_is_any_null = false;
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
      status = core_srt_join_I4_I4_I4(I4_src_lnk, I4_src_val, src_nR, 
	  I4_dst_lnk, I4_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	      ( src_val_rec.fldtype  == I4 ) && 
	      ( dst_lnk_rec.fldtype  == I8 ) ) {
      status = core_srt_join_I4_I4_I8(I4_src_lnk, I4_src_val, src_nR, 
	  I8_dst_lnk, I4_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	      ( src_val_rec.fldtype  == I8 ) && 
	      ( dst_lnk_rec.fldtype  == I4 ) ) {
      status = core_srt_join_I4_I8_I4(I4_src_lnk, I8_src_val, src_nR, 
	  I4_dst_lnk, I8_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I4 ) && 
	      ( src_val_rec.fldtype  == I8 ) && 
	      ( dst_lnk_rec.fldtype  == I8 ) ) {
      status = core_srt_join_I4_I8_I8(I4_src_lnk, I8_src_val, src_nR, 
	  I8_dst_lnk, I8_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	      ( src_val_rec.fldtype  == I4 ) && 
	      ( dst_lnk_rec.fldtype  == I4 ) ) {
      status = core_srt_join_I8_I4_I4(I8_src_lnk, I4_src_val, src_nR, 
	  I4_dst_lnk, I4_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	      ( src_val_rec.fldtype  == I4 ) && 
	      ( dst_lnk_rec.fldtype  == I8 ) ) {
      status = core_srt_join_I8_I4_I8(I8_src_lnk, I4_src_val, src_nR, 
	  I8_dst_lnk, I4_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	      ( src_val_rec.fldtype  == I8 ) && 
	      ( dst_lnk_rec.fldtype  == I4 ) ) {
      status = core_srt_join_I8_I8_I4(I8_src_lnk, I8_src_val, src_nR, 
	  I4_dst_lnk, I8_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else if ( ( src_lnk_rec.fldtype  == I8 ) && 
	      ( src_val_rec.fldtype  == I8 ) && 
	      ( dst_lnk_rec.fldtype  == I8 ) ) {
      status = core_srt_join_I8_I8_I8(I8_src_lnk, I8_src_val, src_nR, 
	  I8_dst_lnk, I8_dst_val, dst_nn, lb, ub, ijoin_op, &t_is_any_null);
    }
    else { 
      status = -1; 
    }
    if ( t_is_any_null == true ) { is_any_null = true; }
  }
  cBYE(status);
  //--------------------------------------------------------
  // Add output field to meta data 
  zero_fld_rec(&dst_val_rec); dst_val_rec.fldtype = src_val_rec.fldtype; 
  status = add_fld(dst_tbl_id, dst_val, ddir_id, fileno, 
		   &dst_val_id, &dst_val_rec);
  cBYE(status);
  if ( ( is_any_null == true ) && ( nn_dst_val_X != NULL ) ) { 
    zero_fld_rec(&nn_dst_val_rec); nn_dst_val_rec.fldtype = I1; 
    status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_val_id, nn_ddir_id,
			 nn_fileno, nn, &nn_dst_val_id, &nn_dst_val_rec);
    cBYE(status);
  }
  else {
    if ( ( nn_ddir_id >= 0 ) && ( nn_fileno >= 0 ) ) {
      status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
    }
  }
 BYE:
  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_val_X, dst_val_nX);
  rs_munmap(nn_dst_val_X, nn_dst_val_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);
  return(status);
}
