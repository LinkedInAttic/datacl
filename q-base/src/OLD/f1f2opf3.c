#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1f2opf3.h"
#include "vec_f1f2opf3.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

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
  FLD_TYPE *f1_meta = NULL, *nn_f1_meta = NULL;
  FLD_TYPE *f2_meta = NULL, *nn_f2_meta = NULL;
  long long nR; 
  int tbl_id = INT_MIN; 
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int f2_id = INT_MIN, nn_f2_id = INT_MIN;
  int f3_id = INT_MIN, nn_f3_id = INT_MIN;
  char *opfile = NULL, *nn_opfile = NULL;
  char str_meta_data[1024];
  int f1type, f2type;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  cBYE(status);
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = chk_if_ephemeral(&f3); cBYE(status);
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); 
  cBYE(status);
  nn_f1_id = g_fld[f1_id].nn_fld_id;
  if ( nn_f1_id >= 0 ) { 
    nn_f1_meta = &(g_fld[nn_f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nn_f1_X, &nn_f1_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, f2, &f2_id); cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  f2_meta = &(g_fld[f2_id]);
  status = rs_mmap(f2_meta->filename, &f2_X, &f2_nX, 0); 
  cBYE(status);
  nn_f2_id = g_fld[f2_id].nn_fld_id;
  if ( nn_f2_id >= 0 ) { 
    nn_f2_meta = &(g_fld[nn_f2_id]);
    status = rs_mmap(nn_f2_meta->filename, &nn_f2_X, &nn_f2_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  status = mk_ifldtype(f1_meta->fldtype, &f1type);
  cBYE(status);
  status = mk_ifldtype(f2_meta->fldtype, &f2type);
  cBYE(status);
  //--------------------------------------------------------
  /* First deal with stuff that is not parallelized */
  if ( ( f1type == FLDTYPE_CHAR_STRING ) && 
       ( f2type == FLDTYPE_CHAR_STRING ) ) {
    /* TODO TO BE COMPLETED 
       if ( strcmp(op, "+") == 0 ) {
       status = f1f2opf3_concat(docroot, db, tbl, f1, f2, f3);
       cBYE(status);
       }
       else {
       go_BYE(-1);
       }
    */
  }
  else if ( ( strcmp(op, "+") == 0 ) || 
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
	    ( strcmp(op, "&") == 0 ) || /* bitwise and */
	    ( strcmp(op, "|") == 0 ) || /* bitwise or  */
	    ( strcmp(op, "^") == 0 ) || /* bitwise xor */
	    ( strcmp(op, "<<") == 0 ) || /* shift left */
	    ( strcmp(op, ">>") == 0 ) /* shift right */
	    ) {
    status = vec_f1f2opf3(nR, f1type, f2type, str_meta_data,
			  f1_X, nn_f1_X, f2_X, nn_f2_X, op, &opfile, &nn_opfile);
    cBYE(status);
    status = add_fld(tbl, f3, str_meta_data, &f3_id);
    cBYE(status);
    if ( nn_opfile != NULL ) { 
      status = add_aux_fld(tbl, f3, nn_opfile, "nn", &nn_f3_id);
      cBYE(status);
    }
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  return(status);
}
