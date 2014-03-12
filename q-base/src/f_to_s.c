#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "set_meta.h"
#include "get_meta.h"
#include "is_fld.h"
#include "mk_file.h"
#include "f_to_s.h"
#include "vec_f_to_s.h"
#include "approx_unique.h"
#include "is_tbl.h"
#include "is_s_in_f.h"
#include "meta_globals.h"
#include "is_sorted_alldef_I1.h"
#include "is_sorted_alldef_I2.h"
#include "is_sorted_alldef_I4.h"
#include "is_sorted_alldef_I8.h"
#include "extract_I8.h"

// last review 9/12/2013

//---------------------------------------------------------------
// START FUNC DECL
int 
f_to_s(
	   const char *tbl,
	   const char *fld,
	   const char *str_op_spec,
	   char *rslt_buf,
	   int sz_rslt_buf
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *X    = NULL; size_t nX    = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  long long nR;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE fld_rec; int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 
#define  BUFLEN 32
  char op[BUFLEN]; bool is_null;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  status = is_fld(tbl, -1, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  cBYE(status);
  if ( fld_id < 0 ) { go_BYE(-1); }
  status = get_data(fld_rec, &X, &nX, false); cBYE(status);
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, false); cBYE(status);
  }
  zero_string(op, BUFLEN);
  status = extract_S(str_op_spec, "op=[", "]", op, BUFLEN, &is_null);
  cBYE(status);
  if ( strcmp(str_op_spec, "is_sorted") == 0 ) { /* not parallelized yet */
    // If you have null values, you cannot be sorted
    if ( nn_fld_id >= 0 ) {
      g_flds[fld_id].srttype = unsorted;
    }
    else if ( g_flds[fld_id].srttype == unknown ) { 
      char *fI1      = (char *)X;
      short *fI2     = (short *)X;
      int *fI4       = (int *)X;
      long long *fI8 = (long long *)X;
      SORT_TYPE srttype;
      switch ( fld_rec.fldtype ) { 
	case I1 : is_sorted_alldef_I1(fI1, nR, &srttype); break;
	case I2 : is_sorted_alldef_I2(fI2, nR, &srttype); break;
	case I4 : is_sorted_alldef_I4(fI4, nR, &srttype); break;
	case I8 : is_sorted_alldef_I8(fI8, nR, &srttype); break;
	default : go_BYE(-1); break;
      }
      g_flds[fld_id].srttype = srttype;
    }
    status = mk_str_srttype(g_flds[fld_id].srttype, rslt_buf);
    cBYE(status);
  }
  else if ( strcmp(str_op_spec, "approx_uq") == 0 ) {
    long long num_uq_I8; double accuracy_F8; int is_good;
    if ( fld_rec.fldtype != I4 ) { go_BYE(-1); }
    status = approx_unique((int *)X, nn_X, nR, &num_uq_I8, &accuracy_F8, &is_good);
    cBYE(status);
    sprintf(rslt_buf, "%lld:%lf:%d", num_uq_I8, accuracy_F8, is_good); 
  }
  else if ( strcmp(op, "get_idx") == 0 ) {
    long long idx = -1;
    char str_srttype[BUFLEN]; zero_string(str_srttype, BUFLEN);
    char str_val[BUFLEN];    zero_string(str_val, BUFLEN);
    status = chk_aux_info(str_op_spec); cBYE(status);
    status = extract_S(str_op_spec, "val=[", "]", str_val, BUFLEN, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    status = int_get_meta(tbl_id, fld_id, "srttype", str_srttype); cBYE(status);
    status = is_s_in_f(X, nn_X, fld_rec.fldtype, str_srttype, 
	str_val, nR, &idx);
    cBYE(status);
    sprintf(rslt_buf, "%lld", idx); 
  }
  else if ( strcmp(op, "get_val") == 0 ) {
    long long idx = -1;
    long long valI8; double valF8;
    status = chk_aux_info(str_op_spec); cBYE(status);
    status = extract_I8(str_op_spec, "idx=[", "]", &idx, &is_null); 
    cBYE(status);
    if ( is_null == true ) { go_BYE(-1); }
    if ( ( idx < 0 ) || ( idx >= nR ) ) { go_BYE(-1); }
    switch ( fld_rec.fldtype ) { 
      case I1 : valI8 = ((char *)X)[idx]; break; 
      case I2 : valI8 = ((short *)X)[idx]; break; 
      case I4 : valI8 = ((int *)X)[idx]; break; 
      case I8 : valI8 = ((long long *)X)[idx]; break; 
      case F4 : valF8 = ((float *)X)[idx]; break; 
      case F8 : valF8 = ((double *)X)[idx]; break; 
      default : go_BYE(-1); break; // TODO P3 Implement SC and SV
    }
    switch ( fld_rec.fldtype ) { 
      case I1 : case I2 : case I4 : case I8 : 
        sprintf(rslt_buf, "%lld", valI8);  
	break;
      case F4 : case F8 : 
        sprintf(rslt_buf, "%lf", valF8);  
	break; 
      default :
	  go_BYE(-1);
	break;
    }
  }
  else {
    bool exists = false;
    if ( ( strcmp(str_op_spec, "min") == 0 ) && 
	( fld_rec.is_min_nn == true ) ) {
      exists = true;
      switch ( fld_rec.fldtype ) { 
	case B : case I1 : case I2 : case I4 : case I8 : 
         sprintf(rslt_buf, "%lld:%lld", fld_rec.minval.minI8, fld_rec.num_nn);
	 break;
	case F4 : case F8 : 
         sprintf(rslt_buf, "%lf:%lld", fld_rec.minval.minF8, fld_rec.num_nn);
	 break;
	default : 
	 go_BYE(-1);
	 break;
      }
    }
    else if ( ( strcmp(str_op_spec, "max") == 0 ) && 
	( fld_rec.is_max_nn == true ) ) {
      exists = true;
      switch ( fld_rec.fldtype ) { 
	case B : case I1 : case I2 : case I4 : case I8 : 
         sprintf(rslt_buf, "%lld:%lld", fld_rec.maxval.maxI8, fld_rec.num_nn);
	 break;
	case F4 : case F8 : 
         sprintf(rslt_buf, "%lf:%lld", fld_rec.maxval.maxF8, fld_rec.num_nn);
	 break;
	default : 
	 go_BYE(-1);
	 break;
      }
    }
    else if ( ( strcmp(str_op_spec, "sum") == 0 ) && 
	( fld_rec.is_sum_nn == true ) ) {
      exists = true;
      switch ( fld_rec.fldtype ) { 
	case B : case I1 : case I2 : case I4 : case I8 : 
         sprintf(rslt_buf, "%lld:%lld", fld_rec.sumval.sumI8, fld_rec.num_nn);
	 break;
	case F4 : case F8 : 
         sprintf(rslt_buf, "%lf:%lld", fld_rec.sumval.sumF8, fld_rec.num_nn);
	 break;
	default : 
	 go_BYE(-1);
	 break;
      }
    }
    if ( exists == false ) { 
       status = vec_f_to_s(X, fld_rec.fldtype, nn_X, nR, str_op_spec, 
			rslt_buf, sz_rslt_buf);
       cBYE(status);
       status = int_set_meta(tbl_id, fld_id, str_op_spec, rslt_buf, true); 
       cBYE(status);
    }
    else {
    }
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return status ;
}
