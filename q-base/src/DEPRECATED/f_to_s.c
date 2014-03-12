#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <values.h>
#include "constants.h" 
#include "macros.h" 
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "fop.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "vec_f_to_s.h"
#include "meta_globals.h"
#include "is_sorted_alldef_I.h"
#include "is_sorted_alldef_L.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f_to_s(
       char *tbl,
       char *fld,
       char *op,
       char *str_result
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t n_nn_X = 0;
  FLD_TYPE f1_meta;
  FLD_TYPE nn_f1_meta; 
  int fldtype, tbl_id, fld_id, nn_fld_id, sorttype; 
  long long nR; 
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  zero_fld_meta(&f1_meta);
  zero_fld_meta(&nn_f1_meta);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  f1_meta = g_fld[fld_id];
  status = rs_mmap(f1_meta.filename, &X, &nX, 0); 
  cBYE(status);
  status = mk_ifldtype(f1_meta.fldtype, &fldtype);
  cBYE(status);
  // Get nn field for source link if if it exists
  nn_fld_id = f1_meta.nn_fld_id;
  if ( nn_fld_id >= 0 ) { 
    status = rs_mmap(g_fld[nn_fld_id].filename, &nn_X, &n_nn_X, 0); 
    cBYE(status);
  } 
  if ( strcmp(op, "is_sorted") == 0 ) {
    sorttype = f1_meta.sorttype;
    if ( sorttype == 2 ) { /* Unknown sort value */   
      if ( nn_X == NULL ) { /* no null value */
	if ( strcmp(f1_meta.fldtype, "int") == 0 ) {
	  is_sorted_alldef_I(X, nR, &sorttype);
	}
	else if ( strcmp(f1_meta.fldtype, "long long") == 0 ) {
	  is_sorted_alldef_L(X, nR, &sorttype);
	}
	else {
	  fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1); 
	}
      }
      else {
	fprintf(stderr, "TO BE IMPLEMENTED\n"); go_BYE(-1); 
      }
      /* Update meta data */
      g_fld[fld_id].sorttype = sorttype;
    }
    switch ( sorttype ) {
    case -1 : strcpy(str_result, "descending"); break;
    case  0 : strcpy(str_result, "unsorted"); break;
    case  1 : strcpy(str_result, "ascending"); break;
    default : go_BYE(-1); break;
    }
  }
  else {
    status = vec_f_to_s(X, nn_X, nR, fldtype, op, str_result);
    cBYE(status);
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, n_nn_X);
  return(status);
}
