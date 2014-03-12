#ifdef IPP
#include "ipps.h"
#endif
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
#include "mk_temp_file.h"
#include "countf.h"
#include "countf_I1.h"
#include "countf_I2.h"
#include "countf_I4.h"
#include "countf_I8.h"
#include "countf_nn_I1.h"
#include "countf_nn_I2.h"
#include "countf_nn_I4.h"
#include "countf_nn_I8.h"
#include "assign_I4.h"
#include "assign_I8.h"
#include "extract_S.h"

extern int g_num_cores;
//---------------------------------------------------------------
// START FUNC DECL
int 
countf(
       char *src_tbl,
       char *src_fld,
       char *fk_dst,
       char *cfld,
       char *dst_tbl,
       char *cnt_fld,
       char *options
       )
// STOP FUNC DECL
{
  int status = 0;
  char *fk_dst_X = NULL;    size_t fk_dst_nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;


  TBL_REC_TYPE src_tbl_rec; int src_tbl_id = -1; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id = -1; 

  FLD_REC_TYPE fk_dst_rec; int fk_dst_id = -1; 
  FLD_REC_TYPE nn_fk_dst_rec; int nn_fk_dst_id; 

  FLD_REC_TYPE src_fld_rec; int src_fld_id = -1; 
  FLD_REC_TYPE nn_src_fld_rec; int nn_src_fld_id; 

  FLD_REC_TYPE cfld_rec; int cfld_id = -1;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;

  FLD_REC_TYPE cnt_fld_rec; int cnt_fld_id; 
  bool is_safe = true;
  long long block_size = 0; int nT = 0;
  long long **partial_cntI8 = NULL; 


  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fk_dst == NULL )  || ( *fk_dst == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( ( options != NULL ) && ( *options != '\0' ) ) {
#define BUFLEN 8
    bool is_null;
    char buffer[BUFLEN]; zero_string(buffer, BUFLEN);
    status = extract_S(options, "safe_mode=[", "]", buffer, BUFLEN, &is_null);
    if ( is_null ) { go_BYE(-1); }
    if ( strcasecmp(buffer, "true") == 0 ) { 
      is_safe = true; 
    }
    else if ( strcasecmp(buffer, "false") == 0 ) { 
      is_safe = false; 
    }
    else {
      go_BYE(-1);
    }
  }
  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  long long dst_nR = g_tbls[dst_tbl_id].nR;
  if ( dst_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  long long src_nR = g_tbls[src_tbl_id].nR;
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, fk_dst, &fk_dst_id, &fk_dst_rec, 
		  &nn_fk_dst_id, &nn_fk_dst_rec); 
  cBYE(status);
  chk_range(fk_dst_id, 0, g_n_fld);
  status = get_data(fk_dst_rec, &fk_dst_X, &fk_dst_nX, 0); cBYE(status);
  if ( nn_fk_dst_id >= 0 ) { go_BYE(-1);  }

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); 
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  /* if src_fld has a nn field, then cannot have a cfld */
  if ( nn_src_fld_id >= 0 ) { 
    if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { go_BYE(-1);  }
    status = get_data(nn_src_fld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }

  if ( ( src_fld_rec.fldtype != I4 ) && ( src_fld_rec.fldtype != I8 ) ) {
    go_BYE(-1); 
  }
  int       *I4_src_ptr = (int       *)src_fld_X;

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); 
    if ( cfld_id < 0 ) { go_BYE(-1); }
    if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
    if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }
  //------------------------------------------------------
  FLD_TYPE cnt_fldtype = I8;
  int ddir_id = -1, fileno = -1; 
  int cnt_fldsz = -1; size_t filesz = 0;

  status = get_fld_sz(cnt_fldtype, &cnt_fldsz); cBYE(status);
  filesz = dst_nR * cnt_fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &op_X, &op_nX, true); cBYE(status);

  // START: Initialize destination count to 0 
  block_size = 0; 
  long long *cntI8 = (long long *)op_X;
  status = partition(dst_nR, 4096, -1, &block_size, &nT); cBYE(status);
  if ( nT > 1 ) {
#pragma omp parallel for 
    for ( int tid = 0; tid < nT; tid++ ) { 
      long long lb = tid * block_size;
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = dst_nR; }
#ifdef IPP
      ippsZero_64s(cntI8+lb, (ub-lb)); 
#else
      assign_const_I8(cntI8+lb, (ub-lb), 0);
#endif
    }
  }
  else {
#ifdef IPP
    ippsZero_64s(cntI8, dst_nR);
#else
    assign_const_I8(cntI8, dst_nR, 0);
#endif
  }
  // STOP: Initialize destination count to 0 
  nT = 0; // reset necessary for free of partial_cntI8
  block_size = 0;
  //------------------------------------------------------
  bool is_serial = true;
  if ( ( src_nR > 1048576 ) && ( ( src_nR / dst_nR ) > 64 ) ) {
    is_serial = false;
    nT = 2;
    for ( ; ; nT *= 2 ) {
      if ( nT >= (1*g_num_cores) ) { break; }
      if ( ( dst_nR * nT * 32 ) > src_nR ) { break; }
    }
    partial_cntI8 = malloc(nT * sizeof(long long *));
    return_if_malloc_failed(partial_cntI8);
    for ( int i = 0; i < nT; i++ ) { 
      partial_cntI8[i] = NULL;
      partial_cntI8[i] = malloc(dst_nR * sizeof(long long));
      return_if_malloc_failed(partial_cntI8[i]);
    }
    block_size = src_nR / nT;
  }

  if ( is_serial == true ) { 
  if ( cfld_id >= 0 ) {
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = countf_nn_I1(I4_src_ptr, (char *)fk_dst_X, src_nR, 
	  cfld_X, cntI8, dst_nR, is_safe); 
      cBYE(status); 
      break;
    case I2 : 
      status = countf_nn_I2(I4_src_ptr, (short *)fk_dst_X, src_nR, 
	  cfld_X, cntI8, dst_nR, is_safe); 
      cBYE(status); 
      break;
    case I4 : 
      status = countf_nn_I4(I4_src_ptr, (int *)fk_dst_X, src_nR, 
	  cfld_X, cntI8, dst_nR, is_safe); 
      cBYE(status); 
      break;
    case I8 : 
      status = countf_nn_I8(I4_src_ptr, (long long *)fk_dst_X, src_nR, 
	  cfld_X, cntI8, dst_nR, is_safe); 
      cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else {
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = countf_I1(I4_src_ptr, (char *)fk_dst_X, src_nR, 
	  cntI8, dst_nR, is_safe); cBYE(status);
      break;
    case I2 : 
      status = countf_I2(I4_src_ptr, (short *)fk_dst_X, src_nR, 
	  cntI8, dst_nR, is_safe); cBYE(status); 
      break;
    case I4 : 
      status = countf_I4(I4_src_ptr, (int *)fk_dst_X, src_nR, 
	  cntI8, dst_nR, is_safe); cBYE(status); 
      break;
    case I8 : 
      status = countf_I8(I4_src_ptr, (long long *)fk_dst_X, src_nR, 
	  cntI8, dst_nR, is_safe); cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  }
  else {
    char      *inI1 = (char *)      fk_dst_X;
    short     *inI2 = (short *)     fk_dst_X;
    int       *inI4 = (int *)       fk_dst_X;
    long long *inI8 = (long long *) fk_dst_X;

#pragma omp parallel for 
    for ( int tid = 0; tid < nT; tid++ ) { 
      assign_const_I8(partial_cntI8[tid], dst_nR, 0);
      long long lb = tid * block_size;
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = src_nR; }
      char      *t_inI1   = inI1 + lb;
      short     *t_inI2   = inI2 + lb;
      int       *t_inI4   = inI4 + lb;
      long long *t_inI8   = inI8 + lb;
      int       *t_src_I4 = I4_src_ptr + lb;
      long long t_src_nR = ub - lb;
      if  ( status == -1 ) { continue; }
      if ( cfld_id >= 0 ) {
        char *t_cfld = cfld_X + lb;
	switch ( fk_dst_rec.fldtype ) {
	case I1 : 
	  status = countf_nn_I1(t_src_I4, t_inI1, t_src_nR, t_cfld, 
	      partial_cntI8[tid], dst_nR, is_safe); 
	  break;
	case I2 : 
	  status = countf_nn_I2(t_src_I4, t_inI2, t_src_nR, t_cfld, 
	      partial_cntI8[tid], dst_nR, is_safe); 
	  break;
	case I4 : 
	  status = countf_nn_I4(t_src_I4, t_inI4, t_src_nR, t_cfld, 
	      partial_cntI8[tid], dst_nR, is_safe); 
	  break;
	case I8 : 
	  status = countf_nn_I8(t_src_I4, t_inI8, t_src_nR, t_cfld, 
	      partial_cntI8[tid], dst_nR, is_safe); 
	  break;
	default : 
	  status = -1;
	  break;
	}
      }
      else {
	switch ( fk_dst_rec.fldtype ) {
	case I1 : 
	  status = countf_I1(t_src_I4, t_inI1, t_src_nR, partial_cntI8[tid],
	      dst_nR, is_safe); 
	  break;
	case I2 : 
	  status = countf_I2(t_src_I4, t_inI2, t_src_nR, partial_cntI8[tid],
	      dst_nR, is_safe); 
	  break;
	case I4 : 
	  status = countf_I4(t_src_I4, t_inI4, t_src_nR, partial_cntI8[tid],
	      dst_nR, is_safe); 
	  break;
	case I8 : 
	  status = countf_I8(t_src_I4, t_inI8, t_src_nR, partial_cntI8[tid],
	      dst_nR, is_safe); 
	  break;
	default : 
	  status = -1;
	  break;
	}
      }
    }
    for ( int tid = 0; tid < nT; tid++ ) { 
      long long *partial_cntI8_tid = partial_cntI8[tid];
      for ( int j = 0; j < dst_nR; j++ ) { 
        cntI8[j] += partial_cntI8_tid[j];
      }
    }
  }

  rs_munmap(op_X, op_nX);
  zero_fld_rec(&cnt_fld_rec); cnt_fld_rec.fldtype = cnt_fldtype; 
  status = add_fld(dst_tbl_id, cnt_fld, ddir_id, fileno, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
 BYE:
  if ( nT > 0 ) {
    for ( int i = 0; i < nT; i++ ) {
      free_if_non_null(partial_cntI8[i]);
    }
    free_if_non_null(partial_cntI8);
  }
  rs_munmap(fk_dst_X, fk_dst_nX);
  rs_munmap(op_X, op_nX);
  rs_munmap(cfld_X, cfld_nX);
  rs_munmap(src_fld, src_fld_nX);
  return(status);
}
