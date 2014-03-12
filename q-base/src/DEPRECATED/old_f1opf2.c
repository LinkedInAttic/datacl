#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "hash_string.h"
#include "f1opf2.h"
#include "f1opf2_hash.h"
#include "f1opf2_shift.h"
#include "f1opf2_cum.h"
/*
#include "conv_int_to_longlong.h"
#include "conv_longlong_to_int.h"
#include "conv_char_to_int.h"
#include "conv_char_to_longlong.h"
*/

//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2(
       char *docroot,
       sqlite3 *in_db,
       char *tbl,
       char *f1,
       char *str_op_spec,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *X = NULL; size_t nX = 0;
  char *sz_X = NULL; size_t sz_nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  FLD_META_TYPE in_fld_meta;
  FLD_META_TYPE sz_fld_meta;
  FLD_META_TYPE nn_fld_meta;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  char *endptr = NULL;
  char *op = NULL; char *str_val = NULL; 
  char *nn_f2 = NULL; 
  char str_meta_data[256];
  int tbl_id, fld_id, hash_len;
  long long nR;
  char *newtype = NULL;
  bool is_any_null; int ival;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_string(str_meta_data, 256);
  zero_fld_meta(&sz_fld_meta);
  zero_fld_meta(&nn_fld_meta);
  zero_fld_meta(&in_fld_meta);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, tbl, &nR, &tbl_id);
  cBYE(status);
  status = fld_meta(docroot, db, tbl, f1, -1, &in_fld_meta);
  cBYE(status);
  status = rs_mmap(in_fld_meta.filename, &X, &nX, 0); 
  cBYE(status);
  fld_id = in_fld_meta.id;
  status = get_aux_field_if_it_exists(docroot, db, tbl, fld_id, "nn",
      &nn_fld_meta, &nn_X, &nn_nX);
  cBYE(status);
  status = get_aux_field_if_it_exists(docroot, db, tbl, fld_id, "sz",
      &sz_fld_meta, &sz_X, &sz_nX);
  cBYE(status);
  //-=======================================================
  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  sprintf(str_meta_data, "filename=%s", opfile);
  status = open_temp_file(&nn_ofp, &nn_opfile);
  cBYE(status);
  //--------------------------------------------------------
  status = extract_name_value(str_op_spec, "op=", ":", &op);
  cBYE(status);
  if ( op == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( strcmp(op, "hash") == 0 ) {
    status = extract_name_value(str_op_spec, "len=", ":", &str_val);
    cBYE(status);
    hash_len = strtol(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    status = f1opf2_hash(nR, X, (int *)sz_X, in_fld_meta.n_sizeof, nn_X,
	hash_len, ofp);
    cBYE(status);
    switch ( hash_len ) { 
      case 4 : strcat(str_meta_data, ":fldtype=int:n_sizeof=4"); break;
      case 8 : strcat(str_meta_data, ":fldtype=long long:n_sizeof=8"); break;
      default : go_BYE(-1); break;
    }
    fclose_if_non_null(ofp);
  }
  else if ( strcmp(op, "not") == 0 ) {
    if ( strcmp(in_fld_meta.fldtype, "char") == 0 ) { 
      status = not(X, nn_X, nR, ofp, nn_ofp, &is_any_null);
      cBYE(status);
      strcat(str_meta_data, ":fldtype=char:n_sizeof=1");
    }
    else {
      go_BYE(-1);
    }
  }
  else if ( strcmp(op, "conv") == 0 ) {
    /* OLD STUFF 
    status = extract_name_value(str_op_spec, "newtype=", ":", &newtype);
    cBYE(status);
    if ( ( newtype == NULL ) || ( *newtype == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(newtype, "int") == 0 ) {
      if ( strcmp(in_fld_meta.fldtype, "long long") == 0 ) {
        status = conv_longlong_to_int(X, nn_X, nR, ofp, nn_ofp, &is_any_null);
        cBYE(status);
      }
      else if ( strcmp(in_fld_meta.fldtype, "char") == 0 ) {
        status = conv_char_to_int(X, nn_X, nR, ofp, nn_ofp, &is_any_null);
        cBYE(status);
      }
      else {
	go_BYE(-1);
      }
      strcat(str_meta_data, ":fldtype=int:n_sizeof=4");
    }
    else if ( strcmp(newtype, "long long") == 0 ) {
      if ( strcmp(in_fld_meta.fldtype, "int") == 0 ) {
      status = conv_int_to_longlong(X, nn_X, nR, ofp, nn_ofp, &is_any_null);
      cBYE(status);
      }
      else if ( strcmp(in_fld_meta.fldtype, "char") == 0 ) {
        status = conv_char_to_longlong(X, nn_X, nR, ofp, nn_ofp, &is_any_null);
        cBYE(status);
      }
      else { 
	go_BYE(-1);
      }
      strcat(str_meta_data, ":fldtype=long long:n_sizeof=8");
    }
    else {
      fprintf(stderr, "Invalid newtype = %s \n", newtype);
      go_BYE(-1);
    }
    */
  }
  else if ( strcmp(op, "shift") == 0 ) {
    status = extract_name_value(str_op_spec, "val=", ":", &str_val);
    cBYE(status);
    ival = strtol(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { 
      fprintf(stderr, "invalid shift specifier = [%s] \n", str_val);
      go_BYE(-1); 
    }
    status = f1opf2_shift(X, nn_X, nR, in_fld_meta.fldtype, ival, ofp, nn_ofp);
    cBYE(status);
    fclose_if_non_null(ofp);
    fclose_if_non_null(nn_ofp);
    sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",opfile,
	in_fld_meta.fldtype, in_fld_meta.n_sizeof);
    is_any_null = true;
  }
  else if ( strcmp(op, "cum") == 0 ) {
    status = f1opf2_cum(X, nR, in_fld_meta.fldtype, FLDTYPE_INT, ofp);
    cBYE(status);
    fclose_if_non_null(ofp);
    sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",opfile,
	in_fld_meta.fldtype, in_fld_meta.n_sizeof);
    is_any_null = false;
  }
  else { 
    go_BYE(-1);
  }
  status = add_fld(docroot, db, tbl, f2, str_meta_data);
  cBYE(status);
  if ( is_any_null ) {
    status = add_aux_fld(docroot, db, tbl, f2, nn_opfile, "nn");
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
    free_if_non_null(nn_opfile);
  }
BYE:
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(sz_X, sz_nX);
  if ( in_db == NULL ) { sqlite3_close(db); }

  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);

  free_if_non_null(op);
  free_if_non_null(newtype);
  free_if_non_null(str_val);

  free_if_non_null(nn_f2);
  return(status);
}

// START FUNC DECL
int
not(
    char *X, 
    char *nn_X, 
    int nR, 
    FILE *ofp, 
    FILE *nn_ofp, 
    bool *ptr_is_any_null
   )
// STOP FUNC DECL
{
  int status = 0;
  char cval, nn;

  *ptr_is_any_null = false;
  for ( int i = 0; i < nR; i++ ) { 
    if ( ( nn_X == NULL ) || ( nn_X[i] == TRUE ) )  {
      if ( X[i] == TRUE ) {
	cval = FALSE;
      }
      else if ( X[i] == FALSE ) {
	cval = TRUE;
      }
      else { 
	go_BYE(-1); 
      }
      nn = TRUE;
    }
    else {
      cval = FALSE;
      nn = FALSE;
      *ptr_is_any_null = true;
    }
    fwrite(&cval, sizeof(char), 1, ofp);
    fwrite(&nn,   sizeof(char), 1, nn_ofp);
  }
  fclose(ofp);
  fclose(nn_ofp);
BYE:
  return(status);
}
