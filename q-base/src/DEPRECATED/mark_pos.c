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
#include "f_to_s.h"
#include "mk_temp_file.h"
#include "count.h"
#include "mark_pos_I1.h"
#include "mark_pos_I2.h"
#include "mark_pos_I4.h"
#include "mark_pos_I8.h"
#include "mark_pos_nn_I1.h"
#include "mark_pos_nn_I2.h"
#include "mark_pos_nn_I4.h"
#include "mark_pos_nn_I8.h"
#include "assign_I4.h"


//---------------------------------------------------------------
// START FUNC DECL
int 
mark_pos(
	 char *src_tbl,
	 char *fk_dst,
	 char *cfld,
	 char *dst_tbl,
	 char *op_fld
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL;    size_t nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;

  TBL_REC_TYPE src_tbl_rec; int src_tbl_id = -1; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id = -1; 

  FLD_REC_TYPE fk_dst_rec; int fk_dst_id = -1; 
  FLD_REC_TYPE nn_fk_dst_rec; int nn_fk_dst_id = -1; 

  FLD_REC_TYPE cfld_rec; int cfld_id = -1;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id = -1;

  FLD_REC_TYPE op_fld_rec; int op_fld_id = -1; 

#define BUFSZ 32
  char rsltbuf[BUFSZ];

  long long src_nR = -1, dst_nR = -1;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fk_dst == NULL ) || ( *fk_dst == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( op_fld == NULL ) || ( *op_fld == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;
  if ( dst_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, fk_dst, &fk_dst_id, &fk_dst_rec, 
		  &nn_fk_dst_id, &nn_fk_dst_rec); 
  cBYE(status);
  chk_range(fk_dst_id, 0, g_n_fld);

  status = get_data(fk_dst_rec, &X, &nX, 0); cBYE(status);
  if ( nn_fk_dst_id >= 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); 
    if ( cfld_id >= 0 ) { 
      if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
      if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
    }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }
  status = f_to_s(src_tbl, fk_dst, "is_sorted", rsltbuf, BUFSZ);
  cBYE(status);
  if ( strcmp(rsltbuf, "ascending") != 0 ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1); 
  }
  //------------------------------------------------------
  FLD_TYPE op_fldtype = I1;
  int ddir_id = -1, fileno = -1;
  int op_fldsz = -1; size_t op_filesz = 0;

  status = get_fld_sz(op_fldtype, &op_fldsz);
  op_filesz = dst_nR * op_fldsz;
  status = mk_temp_file(op_filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &op_X, &op_nX, true); cBYE(status);

  long long block_size; int nT;
  char *opI1 = (char *)op_X;
  status = partition(dst_nR, 4096, -1, &block_size, &nT); cBYE(status);
  cilkfor ( int tid = 0; tid < dst_nR; tid++ ) {
    long long lb = tid * block_size;
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = dst_nR; }
    // Initialize counters to 0 
#ifdef IPP
    ippsZero_64s(opI1+lb, (ub-lb)); 
#else
    for ( int i = lb; i < ub; i++ ) { opI1[i] = 0; }
#endif
  }
  char      *inI1 = (char      *)X;
  short     *inI2 = (short     *)X;
  int       *inI4 = (int       *)X;
  long long *inI8 = (long long *)X;
  //------------------------------------------------------
  //  TODO: Parallelism does not seem to provide any speedup at all

  if ( cfld_id >= 0 ) { 
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = mark_pos_nn_I1(inI1, src_nR, cfld_X, opI1, dst_nR); cBYE(status); 
      break;
    case I2 : 
      status = mark_pos_nn_I2(inI2, src_nR, cfld_X, opI1, dst_nR); cBYE(status); 
      break;
    case I4 : 
      status = mark_pos_nn_I4(inI4, src_nR, cfld_X, opI1, dst_nR); cBYE(status); 
      break;
    case I8 : 
      status = mark_pos_nn_I8(inI8, src_nR, cfld_X, opI1, dst_nR); cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else {
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = mark_pos_I1(inI1, src_nR, opI1, dst_nR); cBYE(status);
      break;
    case I2 : 
      status = mark_pos_I2(inI2, src_nR, opI1, dst_nR); cBYE(status); 
      break;
    case I4 : 
      status = mark_pos_I4(inI4, src_nR, opI1, dst_nR); cBYE(status); 
      break;
    case I8 : 
      status = mark_pos_I8(inI8, src_nR, opI1, dst_nR); cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }

  rs_munmap(op_X, op_nX);
  zero_fld_rec(&op_fld_rec); op_fld_rec.fldtype = op_fldtype; 
  status = add_fld(dst_tbl_id, op_fld, ddir_id, fileno, &op_fld_id, &op_fld_rec);
  cBYE(status);
 BYE:
  rs_munmap(X, nX);
  rs_munmap(op_X, op_nX);
  return(status);
}
