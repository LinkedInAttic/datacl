#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1f2opf3.h"
#include "vec_f1f2opf3.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "get_type_op_fld.h"
#include "meta_globals.h"

// last review 9/5/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
f1f2opf3(
	 char *tbl,
	 char *f1,
	 char *f2,
	 char *op,
	 char *f3
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  FLD_REC_TYPE f3_rec, nn_f3_rec;
  long long nR; 
  TBL_REC_TYPE tbl_rec;
  int tbl_id = INT_MIN; 
  FLD_TYPE f3type;
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int f2_id = INT_MIN, nn_f2_id = INT_MIN;
  int f3_id = INT_MIN, nn_f3_id = INT_MIN;
  char *op_X = NULL;    size_t n_op_X = 0;
  char *nn_op_X = NULL; size_t n_nn_op_X = 0;
  int ddir_id = -1, fileno = -1;
  int nn_ddir_id = -1, nn_fileno = -1;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  if ( nn_f2_id >= 0 ) { 
    status = get_data(nn_f2_rec, &nn_f2_X, &nn_f2_nX, false); 
    cBYE(status);
  }
  //--------------------------------------------------------
  if ( ( strcmp(op, "+") == 0 ) || 
       ( strcmp(op, "-") == 0 ) || 
       ( strcmp(op, "*") == 0 ) || 
       ( strcmp(op, "/") == 0 ) || 
       ( strcmp(op, "&&") == 0 ) || 
       ( strcmp(op, "||") == 0 ) || 
       ( strcmp(op, ">") == 0 ) || 
       ( strcmp(op, "<") == 0 ) || 
       ( strcmp(op, ">=") == 0 ) || 
       ( strcmp(op, "<=") == 0 ) || 
       ( strcmp(op, "!=") == 0 ) || 
       ( strcmp(op, "==") == 0 ) ||
       ( strcmp(op, "concat") == 0 ) ||
       ( strcmp(op, "&&!") == 0 ) || /* a and not b */
       ( strcmp(op, "&") == 0 ) || /* bitwise and */
       ( strcmp(op, "|") == 0 ) || /* bitwise or  */
       ( strcmp(op, "^") == 0 ) || /* bitwise xor */
       ( strcmp(op, "<<") == 0 ) || /* shift left */
       ( strcmp(op, ">>") == 0 ) /* shift right */
       ) {
    /*-START: Allocate space ------------------------------*/
    status = get_type_op_fld("f1f2opf3", op, f1_rec.fldtype, f2_rec.fldtype, 
			     "", &f3type);
    cBYE(status);
    int fldsz= 0; size_t filesz  = 0;
    if ( f3type == B ) { 
      status = get_file_size_B(nR, &filesz); cBYE(status);
    }
    else {
      int fldsz;
      status = get_fld_sz(f3type, &fldsz); cBYE(status);
      filesz = fldsz * nR;
    }
    status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
    status = q_mmap(ddir_id, fileno, &op_X, &n_op_X, 1); cBYE(status);

    if ( ( nn_f1_X != NULL ) || ( nn_f2_X != NULL ) ) {
      status = get_fld_sz(I1, &fldsz); cBYE(status);
      filesz = fldsz * nR;
      status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
      status = q_mmap(nn_ddir_id, nn_fileno, &nn_op_X, &n_nn_op_X, true); 
      cBYE(status);
    }
    /*-STOP: Allocate space ------------------------------*/
    status = vec_f1f2opf3(nR, f1_rec.fldtype, f2_rec.fldtype, f1_X, nn_f1_X, 
			  f2_X, nn_f2_X, op, f3type, op_X, nn_op_X);
    cBYE(status);
    zero_fld_rec(&f3_rec); f3_rec.fldtype = f3type;
    status = add_fld(tbl_id, f3, ddir_id, fileno, &f3_id, &f3_rec); cBYE(status);
    if ( nn_fileno > 0 ) {
      zero_fld_rec(&nn_f3_rec); nn_f3_rec.fldtype = I1;
      status = add_aux_fld(NULL, tbl_id, NULL, f3_id, nn_ddir_id, nn_fileno, 
	  nn, &nn_f3_id, &nn_f3_rec);
      cBYE(status);
    }
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
 BYE:
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  return(status);
}
