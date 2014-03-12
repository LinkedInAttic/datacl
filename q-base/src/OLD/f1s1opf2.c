#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "get_nR.h"
#include "f1s1opf2.h"
#include "vec_f1s1opf2.h"
#include "aux_fld_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;

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
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *sz_f1_X = NULL; size_t sz_f1_nX = 0;
  FLD_TYPE *f1_meta = NULL, *nn_f1_meta = NULL, *sz_f1_meta = NULL;
  long long nR; 
  int tbl_id, f1_id, nn_f1_id, sz_f1_id;
  int         f2_id, nn_f2_id;
  char *opfile = NULL, *nn_opfile = NULL;
  char str_meta_data[1024];
  int f1type;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = chk_if_ephemeral(&f2); cBYE(status);
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta  = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); cBYE(status);
  // Get nn field for f1 if if it exists
  nn_f1_id = f1_meta->nn_fld_id;
  if ( nn_f1_id >= 0 ) { 
    nn_f1_meta = &(g_fld[nn_f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nn_f1_X, &nn_f1_nX, 0); 
    cBYE(status);
  } 
  // Get sz field for f1 if if it exists
  sz_f1_id = f1_meta->sz_fld_id;
  if ( sz_f1_id >= 0 ) { 
    sz_f1_meta = &(g_fld[sz_f1_id]);
    status = rs_mmap(sz_f1_meta->filename, &sz_f1_X, &sz_f1_nX, 0); 
    cBYE(status);
  } 
  status = mk_ifldtype(f1_meta->fldtype, &f1type);
  cBYE(status);
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
            ( strcmp(op, ">>") == 0 ) 
	    ) {
    status = vec_f1s1opf2(nR, f1type, str_meta_data,
	f1_X, nn_f1_X, sz_f1_X, str_scalar, op, &opfile, &nn_opfile);
    cBYE(status);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
    if ( nn_opfile != NULL ) { 
      status = add_aux_fld(tbl, f2, nn_opfile, "nn", &nn_f2_id);
      cBYE(status);
    }
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
BYE:
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(sz_f1_X, sz_f1_nX);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  return(status);
}
