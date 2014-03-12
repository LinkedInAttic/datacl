#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "get_nR.h"
#include "f1s1opf2.h"
#include "vec_f1s1opf2.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "get_type_op_fld.h"
#include "mk_temp_file.h"

// last review 9/7/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
f1s1opf2(
       char *tbl,
       char *f1,
       char *str_scalar,
       char *op,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  long long nR; 
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE f1_rec, nn_f1_rec; int f1_id, nn_f1_id;
  FLD_REC_TYPE f2_rec, nn_f2_rec; int f2_id, nn_f2_id;
  int ddir_id    = -1, fileno    = -1;
  int nn_ddir_id = -1, nn_fileno = -1;
  FLD_TYPE f1_fldtype = undef_fldtype, f2_fldtype = undef_fldtype; 
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Get nn field for f1 if if it exists
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); 
    cBYE(status);
  } 
  f1_fldtype = f1_rec.fldtype; 
  status = get_type_op_fld("f1s1opf2", op, f1_fldtype, f1_fldtype, "", &f2_fldtype);
  cBYE(status);
  int fldsz = INT_MAX; size_t filesz = LLONG_MAX;
  /*------------------------------------------ */
  status = get_fld_sz(f2_fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f2_X, &f2_nX, true); cBYE(status);
  /*--------------------------------------------*/
  if ( nn_f1_X == NULL ) { 
    /* No need for nn file */
  }
  else {
    status = get_fld_sz(I1, &fldsz);
    filesz = fldsz * nR; 
    status = mk_temp_file(filesz, &nn_ddir_id, &nn_fileno); 
    cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &nn_f2_X, &nn_f2_nX, 1); 
    cBYE(status);
  }
  //--------------------------------------------------------
  if ( ( strcmp(op, "+") == 0 ) || 
            ( strcmp(op, "-") == 0 ) || 
            ( strcmp(op, "*") == 0 ) || 
            ( strcmp(op, "/") == 0 ) || 
            ( strcmp(op, "%") == 0 ) || 
            ( strcmp(op, "&") == 0 ) || 
            ( strcmp(op, "|") == 0 ) || 
            ( strcmp(op, "^") == 0 ) || 
            ( strcmp(op, ">") == 0 ) || 
            ( strcmp(op, "<") == 0 ) || 
            ( strcmp(op, ">=") == 0 ) || 
            ( strcmp(op, "<=") == 0 ) || 
            ( strcmp(op, "!=") == 0 ) || 
            ( strcmp(op, "==") == 0 ) ||
            ( strcmp(op, "<<") == 0 ) ||
            ( strcmp(op, ">>") == 0 ) || 
           ( strcmp(op, "<||>") == 0 ) || 
	   ( strcmp(op, "<=||>=") == 0 ) || 
	   ( strcmp(op, ">&&<") == 0 ) || 
	   ( strcmp(op, ">=&&<=") == 0 ) 
	    ) {
	// all is well 
    status = vec_f1s1opf2(nR, f1_rec.fldtype, f1_X, nn_f1_X, 
	str_scalar, op, f2_X, nn_f2_X, f2_fldtype);
    cBYE(status);
    zero_fld_rec(&f2_rec); f2_rec.fldtype = f2_fldtype;
    status = add_fld(tbl_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
    cBYE(status);
    if ( nn_fileno > 0 ) { /* there is an nn file */
      zero_fld_rec(&nn_f2_rec); nn_f2_rec.fldtype = I1; 
      status = add_aux_fld(NULL, tbl_id, NULL, f2_id, nn_ddir_id, nn_fileno,
	  nn, &nn_f2_id, &nn_f2_rec);
      cBYE(status);
    }
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
BYE:
  rs_munmap(f1_X,    f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X,    f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  return status ;
}
