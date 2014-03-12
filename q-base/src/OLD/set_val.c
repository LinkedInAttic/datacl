#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

#define FLDTYPE_WCHAR_T 100
#define FLDTYPE_CHAR    200
#define DBG_BUF_LEN 32
//---------------------------------------------------------------
// START FUNC DECL
int 
set_val(
       char *tbl,
       char *fld,
       char *str_idx,
       char *str_val
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0; 

  int tbl_id, fld_id, nn_fld_id;
  long long idx, nR;
  char *endptr = NULL; 
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = g_tbl[tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  if ( fld_id < 0 ) { 
    fprintf(stderr, "Field [%s] not in Table [%s] \n", fld, tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  status = rs_mmap(g_fld[fld_id].filename, &X, &nX, 1); // WRITABLE
  cBYE(status);
  //--------------------------------------------------------
  // Get nn field for source link if if it exists
  nn_fld_id = g_fld[fld_id].nn_fld_id;
  if ( nn_fld_id >= 0 ) { 
    chk_range(nn_fld_id, 0, g_n_fld);
    status = rs_mmap(g_fld[nn_fld_id].filename, &nn_X, &nn_nX, 1); // WRITABLE
    cBYE(status);
  }
  //--------------------------------------------------------
  // Convert idx to long long
  idx = strtoll(str_idx, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( ( idx < 0 ) || ( idx >= nR ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( strcmp(g_fld[fld_id].fldtype, "int") == 0 ) {
    int ival; int *iptr; 
    ival = strtol(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    iptr = (int *)X;
    iptr[idx] = ival;
    if ( nn_X != NULL ) {
      nn_X[idx] = TRUE;
    }
  }
  else if ( strcmp(g_fld[fld_id].fldtype, "long long") == 0 ) {
    long long llval; long long *llptr; 
    llval = strtoll(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    llptr = (long long *)X;
    llptr[idx] = llval;
    if ( nn_X != NULL ) {
      nn_X[idx] = TRUE;
    }
  }
  else if ( strcmp(g_fld[fld_id].fldtype, "bool") == 0 ) {
    char bval; char *bptr; 
    if ( strcmp(str_val, "true") == 0 ) {
      bval = 1;
    }
    else if ( strcmp(str_val, "false") == 0 ) {
      bval = 0;
    }
    else {
      bval = strtol(str_val, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      if ( ( bval != 0 ) && ( bval != 1 ) )  { go_BYE(-1); }
    }
    bptr = (char *)X;
    bptr[idx] = bval;
    if ( nn_X != NULL ) {
      nn_X[idx] = TRUE;
    }
  }
  else if ( strcmp(g_fld[fld_id].fldtype, "float") == 0 ) {
    float fval; float *fptr;
    fval = strtof(str_val, &endptr);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    fptr = (float *)X;
    fptr[idx] = fval;
    if ( nn_X != NULL ) {
      nn_X[idx] = TRUE;
    }
  }
  else if ( strcmp(g_fld[fld_id].fldtype, "double") == 0 ) {
    double dval; double *dptr;
    dval = strtod(str_val, &endptr);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    dptr = (double *)X;
    dptr[idx] = dval;
    if ( nn_X != NULL ) {
      nn_X[idx] = TRUE;
    }
  }
  else {
    fprintf(stderr, "Not implemented for this field type\n");
    go_BYE(-1);
  }
  // TODO: Need to make sure that if nn field is no longer needed, then
  // we delete the field 
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return(status);
}
