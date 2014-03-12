/* READ */
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
#include "countT.h"
#include "count_I1.h"
#include "count_I2.h"
#include "count_I4.h"
#include "count_I8.h"
#include "count_nn_I1.h"
#include "count_nn_I2.h"
#include "count_nn_I4.h"
#include "count_nn_I8.h"
#include "assign_I4.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
countT(
       char *src_tbl,
       char *fk_dst,
       char *aux_tbl,
       char *lb_fld,
       char *ub_fld,
       char *dst_tbl,
       char *cnt_fld
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;    size_t nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;


  TBL_REC_TYPE src_tbl_rec; int src_tbl_id; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id; 
  TBL_REC_TYPE aux_tbl_rec; int aux_tbl_id; 

  FLD_REC_TYPE fk_dst_rec; int fk_dst_id; 
  FLD_REC_TYPE nn_fk_dst_rec; int nn_fk_dst_id; 

  FLD_REC_TYPE lb_fld_rec; int lb_fld_id; 
  FLD_REC_TYPE nn_lb_fld_rec; int nn_lb_fld_id; 

  FLD_REC_TYPE ub_fld_rec; int ub_fld_id; 
  FLD_REC_TYPE nn_ub_fld_rec; int nn_ub_fld_id; 

  FLD_REC_TYPE cnt_fld_rec; int cnt_fld_id; 

  long long src_nR, dst_nR, aux_nR;
#define MAX_LEN 32
  char str_dst_nR[MAX_LEN];

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fk_dst == NULL ) || ( *fk_dst == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( aux_tbl == NULL ) || ( *aux_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld == NULL ) || ( *lb_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_fld == NULL ) || ( *ub_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, aux_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(aux_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(str_dst_nR, MAX_LEN);
  //--------------------------------------------------------
  status = is_tbl(aux_tbl, &aux_tbl_id, &aux_tbl_rec); cBYE(status);
  chk_range(aux_tbl_id, 0, g_n_tbl);
  aux_nR = aux_tbl_rec.nR;
  if ( aux_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, aux_tbl_id, lb_fld, &lb_fld_id, &lb_fld_rec, 
		  &nn_lb_fld_id, &nn_lb_fld_rec); 
  cBYE(status);
  if ( lb_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_lb_fld_id >= 0 ) {  go_BYE(-1); }
  status = get_data(lb_fld_rec, &lb_X, &lb_nX, 0); cBYE(status);

  status = is_fld(NULL, aux_tbl_id, ub_fld, &ub_fld_id, &ub_fld_rec, 
		  &nn_ub_fld_id, &nn_ub_fld_rec); 
  cBYE(status);
  if ( ub_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_ub_fld_id >= 0 ) {  go_BYE(-1); }
  status = get_data(ub_fld_rec, &ub_X, &ub_nX, 0); cBYE(status);

  switch ( lb_fld_rec.fldtype ) { 
    case I4 : case I8 : break;
    default : go_BYE(-1); break;
  }
  if ( lb_fld_rec.fldtype != ub_fld_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  if ( dst_tbl_id < 0 ) { go_BYE(-1); }
  dst_nR = dst_tbl_rec..nR;
  if ( dst_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  if ( src_tbl_id < 0 ) { go_BYE(-1); }
  src_nR = src_tbl_rec.nR;
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, fk_dst, &fk_dst_id, &fk_dst_rec, 
		  &nn_fk_dst_id, &nn_fk_dst_rec); 
  cBYE(status);
  if ( fk_dst_id < 0 ) { go_BYE(-1); }
  status = get_data(fk_dst_rec, &X, &nX, 0); cBYE(status);
  if ( nn_fk_dst_id >= 0 ) {  go_BYE(-1); }
  //------------------------------------------------------
  int fldsz; size_t filesz = 0;
  int ddir_id = -1, fileno = -1; 
  status = get_fld_sz(I4, &fldsz); 
  filesz = fldsz * dst_nR;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &op_X, &op_nX, true); cBYE(status);
  int *cntI4 = (int *)op_X;
  //------------------------------------------------------
  if ( dst_nR > INT_MAX ) { go_BYE(-1); } /* required by count_In */
  // Initialize counters to 0 
#ifdef IPP
  ippsZero_32s(cntI4[tid], dst_nR); 
#else
  assign_const_I4(cntI4, dst_nR, 0); 
#endif

  int *I4_aux_lb_ptr = (int *)lb_X;
  int *I4_aux_ub_ptr = (int *)ub_X;
  long long *I8_aux_lb_ptr = (long long *)lb_X;
  long long *I8_aux_ub_ptr = (long long *)ub_X;
  FLD_TYPE aux_fldtype = lb_fld_rec.fldtype;

  long long in_lb, in_ub;
  char *I1_src_ptr; short *I2_src_ptr; int *I4_src_ptr;
  int src_val;

  // cilkfor does no good here and causes marginal degradation
    for ( long long i = 0; i < aux_nR; i++ ) {
      switch ( aux_fldtype ) { 
      case I4 : in_lb = I4_aux_lb_ptr[i]; in_ub = I4_aux_ub_ptr[i]; break;
      case I8 : in_lb = I8_aux_lb_ptr[i]; in_ub = I8_aux_ub_ptr[i]; break;
      default : go_BYE(1); break;
      }
      switch ( fk_dst_rec.fldtype ) { 
      case I1 : 
        I1_src_ptr = (char *)X; I1_src_ptr += in_lb;
	for ( long long j = in_lb; j < in_ub; j++ ) { 
	  src_val = *I1_src_ptr++;
	  cntI4[src_val]++;
	}
	break;
      case I2 : 
	I2_src_ptr = (short *)X; I2_src_ptr += in_lb;
	for ( long long j = in_lb; j < in_ub; j++ ) { 
	  src_val = *I2_src_ptr++;
	  cntI4[src_val]++;
	}
	break;
      case I4 : 
        I4_src_ptr = (int *)X; I4_src_ptr += in_lb;
	for ( long long j = in_lb; j < in_ub; j++ ) { 
	  src_val = *I4_src_ptr++;
	  cntI4[src_val]++;
	}
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  rs_munmap(op_X, op_nX);
  zero_fld_rec(&cnt_fld_rec); cnt_fld_rec.fldtype = I4; 
  status = add_fld(dst_tbl_id, cnt_fld, ddir_id, fileno, 
		   &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
 BYE:
  rs_munmap(X, nX);
  rs_munmap(op_X, op_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  return(status);
}
