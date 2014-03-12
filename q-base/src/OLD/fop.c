#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "set_fld_info.h"

#include "sort_asc_float.h"
/* OLD 
#include "sort_asc_int.h"
#include "sort_asc_longlong.h"
#include "sort_desc_int.h"
#include "sort_desc_longlong.h"
*/
#include "qsort_asc_int.h"
#include "qsort_asc_longlong.h"
#include "qsort_dsc_int.h"
#include "qsort_dsc_longlong.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
fop(
       char *tbl,
       char *fld,
       char *str_op_spec
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  FLD_TYPE x_fld_meta;
  int tbl_id, fld_id;
  long long nR; 

  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  x_fld_meta = g_fld[fld_id];
  if ( x_fld_meta.is_external ) { go_BYE(-1); }

  status = rs_mmap(x_fld_meta.filename, &X, &nX, 1); // modifying
  cBYE(status);
  if ( nX == 0 ) { go_BYE(-1); }
  if ( strcmp(str_op_spec, "sortA") == 0 ) {
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) {
      // OLD qsort(X, nR, sizeof(int), sort_asc_int);
      qsort_asc_int(X, nR, sizeof(int), NULL);
    }
    else if ( strcmp(x_fld_meta.fldtype, "float") == 0 ) {
      qsort(X, nR, sizeof(int), sort_asc_float);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) {
      // OLD qsort(X, nR, sizeof(long long), sort_asc_longlong);
      qsort_asc_longlong(X, nR, sizeof(long long), NULL);
    }
    else { 
      fprintf(stderr, "Not implemented\n"); go_BYE(-1);
    }
    set_fld_info(tbl, fld, "sort=1");
  }
  else if ( strcmp(str_op_spec, "sortD") == 0 ) { 
    if ( strcmp(x_fld_meta.fldtype, "int") == 0 ) {
      // OLD qsort(X, nR, sizeof(int), sort_desc_int);
      qsort_dsc_int(X, nR, sizeof(int), NULL);
    }
    else if ( strcmp(x_fld_meta.fldtype, "long long") == 0 ) {
      // OLD qsort(X, nR, sizeof(long long), sort_desc_longlong);
      qsort_dsc_longlong(X, nR, sizeof(long long), NULL);
    }
    else { 
      fprintf(stderr, "Not implemented\n");
      go_BYE(-1);
    }
    set_fld_info(tbl, fld, "sort=-1");
  }
  else if ( strcmp(str_op_spec, "lcase") == 0 ) { 
    char c;
    if ( strcmp(x_fld_meta.fldtype, "char string") != 0 ) {
      fprintf(stderr, "Field [%s] in Table [%s] has type [%s] \n",
	  fld, tbl, x_fld_meta.fldtype);
      fprintf(stderr, "Needs to be 'char string' \n");
      go_BYE(-1);
    }
    for ( size_t ix = 0; ix < nX; ix++ ) { 
      c = X[ix];
      if ( isalpha(c) ) {
	X[ix] = tolower(c);
      }
    }

  }
  else {
    char *op = NULL;
    status = extract_name_value(str_op_spec, "op=", ":", &op);
    cBYE(status);
    if ( op == NULL ) { go_BYE(-1); }
    if ( strcmp(op, "cast") == 0 ) {
      char *newtype = NULL;
      status = extract_name_value(str_op_spec, "newtype=", ":", &newtype);
      cBYE(status);
      if ( newtype == NULL ) { go_BYE(-1); }
      if ( ( strcmp(newtype, "int") == 0 ) || 
	  ( strcmp(newtype, "float") == 0 ) ||
	  ( strcmp(newtype, "double") == 0 ) ||
	  ( strcmp(newtype, "long long") == 0 ) ) {
	/* All is well */
      }
      else {
	fprintf(stderr, "newtype = [%s] is invalid \n", newtype);
	go_BYE(-1);
      }
      strcpy(g_fld[fld_id].fldtype, newtype);
      free_if_non_null(newtype);
    }
    else {
      fprintf(stderr, "Unknown operation = %s \n", op);
      go_BYE(-1);
    }
    free_if_non_null(op);
  }
  //-------------------------------------------------------
BYE:
  rs_munmap(X, nX);
  return(status);
}
