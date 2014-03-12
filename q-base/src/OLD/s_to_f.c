#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "s_to_f.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "meta_data.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
s_to_f(
       char *tbl,
       char *fld,
       char *str_scalar
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char *sz_X = NULL; size_t sz_nX = 0;
  FILE  *ofp = NULL; char *opfile = NULL;
  FILE  *nn_ofp = NULL; char *nn_opfile = NULL;
  FILE  *sz_ofp = NULL; char *sz_opfile = NULL;
  char *fldtype = NULL; char *op = NULL;
  char *start = NULL; char *incr = NULL; char *val = NULL;
  long long nR; 
  int tbl_id, fld_id; 
  char *endptr;
  int istart, iincr, ival; char cval; float fval;
  int ll_start, ll_incr; long long ll_val; 
  int n_sizeof  = INT_MIN;
  char str_meta_data[1024];
  char buf[1024]; bool is_any_null = false; char nn_val;
  TBL_REC_TYPE tbl_rec;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(buf, 1024);
  //--------------------------------------------------------
  status  = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  if ( nR <= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = extract_name_value(str_scalar, "fldtype=", ":", &fldtype);
  cBYE(status);
  if ( ( fldtype == NULL ) || ( *fldtype == '\0' ) ) { go_BYE(-1); }
  status = extract_name_value(str_scalar, "op=", ":", &op);
  cBYE(status);
  //--------------------------------------------------------
  if ( strcmp(fldtype, "int" ) == 0 ) {
    n_sizeof = sizeof(int);
  }
  else if ( strcmp(fldtype, "bool" ) == 0 ) {
    n_sizeof = sizeof(char);
  }
  else if ( strcmp(fldtype, "long long" ) == 0 ) {
    n_sizeof = sizeof(long long);
  }
  else if ( strcmp(fldtype, "float" ) == 0 ) {
    n_sizeof = sizeof(float);
  }
  else if ( strcmp(fldtype, "char string" ) == 0 ) {
    n_sizeof = 0; 
  }
  else { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( n_sizeof > 0 ) { 
    long long filesz = nR * n_sizeof;
    status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(opfile, filesz); cBYE(status);
    status = rs_mmap(opfile, &X, &nX, 1);
  }

  if ( strcmp(fldtype, "int") == 0 ) {
    int *iptr = (int *)X;
    // Create meta data string 
    zero_string_to_nullc(buf);
    sprintf(buf, "n_sizeof=%lu:", sizeof(int));
    strcat(str_meta_data, buf); 
    if ( strcmp(op, "seq") == 0 ) {
      status = extract_name_value(str_scalar, "start=", ":", &start);
      cBYE(status);
      if ( ( start == NULL ) || ( *start == '\0' ) ) { go_BYE(-1); }
      status = extract_name_value(str_scalar, "incr=", ":", &incr);
      cBYE(status);
      if ( ( incr == NULL ) || ( *incr == '\0' ) ) { go_BYE(-1); }

      istart = strtol(start, &endptr, 10);
      iincr  = strtol(incr , &endptr, 10);
      ival   = istart;
      for ( long long i = 0; i < nR; i++ ) { 
	iptr[i] = ival;
	ival += iincr;
      }
      fclose_if_non_null(ofp);
      free_if_non_null(start);
      free_if_non_null(incr);
    }
    else if ( strcmp(op, "const") == 0 ) {
      status = extract_name_value(str_scalar, "val=", ":", &val);
      cBYE(status);
      if ( ( val == NULL ) || ( *val == '\0' ) ) { go_BYE(-1); }
      if ( strcmp(val, "undef") == 0 ) {
	ival = 0; nn_val = FALSE;
        status = open_temp_file(&nn_ofp, &nn_opfile, 0);
        cBYE(status);
	/* TODO This is a candidate for optimization */
        for ( int i = 0; i < nR; i++ ) { 
          fwrite(&nn_val, 1, sizeof(char), nn_ofp);
        }
        fclose_if_non_null(nn_ofp);
      }
      else {
        ival  = strtol(val , &endptr, 10);
      }
      for ( long long i = 0; i < nR; i++ ) { 
	iptr[i] = ival;
      }
      fclose_if_non_null(ofp);
      free_if_non_null(val);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fldtype, "float") == 0 ) {
    float *fptr = (float *)X;
    // Create meta data string 
    zero_string_to_nullc(buf);
    sprintf(buf, "n_sizeof=%lu:", sizeof(float));
    strcat(str_meta_data, buf); 
    if ( strcmp(op, "const") == 0 ) {
      status = extract_name_value(str_scalar, "val=", ":", &val);
      cBYE(status);
      if ( ( val == NULL ) || ( *val == '\0' ) ) { go_BYE(-1); }
      if ( strcmp(val, "undef") == 0 ) {
	fval = 0; nn_val = FALSE;
        status = open_temp_file(&nn_ofp, &nn_opfile, 0);
        cBYE(status);
        for ( long long i = 0; i < nR; i++ ) { 
          fwrite(&nn_val, 1, sizeof(char), nn_ofp);
        }
        fclose_if_non_null(nn_ofp);
      }
      else {
        fval  = strtod(val , &endptr);
      }
      for ( long long i = 0; i < nR; i++ ) { 
        fptr[i] = fval;
      }
      fclose_if_non_null(ofp);
      free_if_non_null(val);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fldtype, "bool") == 0 ) {
    bool *bptr = (bool *)X;
    if ( strcmp(op, "const") == 0 ) {
      status = extract_name_value(str_scalar, "val=", ":", &val);
      cBYE(status);
      if ( ( val == NULL ) || ( *val == '\0' ) ) { go_BYE(-1); }
      if ( strcmp(val, "true") == 0 ) { cval = 1; } 
      else if ( strcmp(val, "false") == 0 ) { cval = 0; } 
      else { 
	fprintf(stderr, "bool value must be true or false \n");
	go_BYE(-1);
      }
      for ( long long i = 0; i < nR; i++ ) { 
        bptr[i] = cval;
      }
      fclose_if_non_null(ofp);
      // Create meta data string 
      zero_string_to_nullc(buf);
      sprintf(buf, "n_sizeof=%lu:", sizeof(bool));
      strcat(str_meta_data, buf); 
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fldtype, "long long") == 0 ) {
    long long *lptr = (long long *)X;
    // Create meta data string 
    zero_string_to_nullc(buf);
    sprintf(buf, "n_sizeof=%lu:", sizeof(long long));
    strcat(str_meta_data, buf); 
    if ( strcmp(op, "seq") == 0 ) {
      status = extract_name_value(str_scalar, "start=", ":", &start);
      cBYE(status);
      if ( ( start == NULL ) || ( *start == '\0' ) ) { go_BYE(-1); }
      status = extract_name_value(str_scalar, "incr=", ":", &incr);
      cBYE(status);
      if ( ( incr == NULL ) || ( *incr == '\0' ) ) { go_BYE(-1); }

      ll_start = strtoll(start, &endptr, 10);
      ll_incr  = strtoll(incr , &endptr, 10);
      ll_val   = ll_start;
      for ( long long i = 0; i < nR; i++ ) { 
	lptr[i] = ll_val;
	ll_val += ll_incr;
      }
      fclose_if_non_null(ofp);
      free_if_non_null(start);
      free_if_non_null(incr);
    }
    else if ( strcmp(op, "const") == 0 ) {
      status = extract_name_value(str_scalar, "val=", ":", &val);
      cBYE(status);
      if ( ( val == NULL ) || ( *val == '\0' ) ) { go_BYE(-1); }
      ll_val  = strtoll(val , &endptr, 10);
      for ( long long i = 0; i < nR; i++ ) { 
	lptr[i] = ll_val;
      }
      fclose_if_non_null(ofp);
      free_if_non_null(val);
    }
    else {
      fprintf(stderr, "NOT IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
  else if ( strcmp(fldtype, "char string") == 0 ) {
    // Create meta data string 
    zero_string_to_nullc(buf);
    strcpy(buf, "n_sizeof=0:");
    strcat(str_meta_data, buf); 
    if ( strcmp(op,"const") == 0 ) { 
      is_any_null = false;
      status = extract_name_value(str_scalar, "val=", ":", &val);
      cBYE(status);
      int len = strlen(val);
      char nullc = '\0';
      status = open_temp_file(&ofp, &opfile, 0); cBYE(status);

      long long filesz = sizeof(int) * nR;
      status = open_temp_file(&sz_ofp, &sz_opfile, filesz); cBYE(status);
      fclose_if_non_null(sz_ofp);
      status = mk_file(sz_opfile, filesz); cBYE(status);
      status = rs_mmap(sz_opfile, &sz_X, &sz_nX, 1); // writing 
      int *szptr = (int *)sz_X;
      for ( long long i = 0; i < nR; i++ ) { 
	szptr[i] = len + 1; // +1 for nullc
	fwrite(val, sizeof(char), len, ofp);
	fwrite(&nullc, sizeof(char), 1, ofp);
      }
      rs_munmap(sz_X, sz_nX);
      fclose_if_non_null(ofp);
      free_if_non_null(val);
    } 
    else { 
      go_BYE(-1);
    }
  }
  else {
    fprintf(stderr, "NOT IMPLEMENTED\n");
    go_BYE(-1);
  }
  // Complete meta data string 
  zero_string_to_nullc(buf);
  if ( strcmp(fldtype, "bool") ==  0 ) { 
    sprintf(buf, "fldtype=bool:");
  }
  else {
    sprintf(buf, "fldtype=%s:", fldtype);
  }
  strcat(str_meta_data, buf); 

  zero_string_to_nullc(buf);
  sprintf(buf, "filename=%s:", opfile);
  strcat(str_meta_data, buf); 

  //--------------------------------------------------------
  // Add to meta data store
  status = add_fld(tbl, fld, str_meta_data, &fld_id);
  cBYE(status);
  if ( strcmp(fldtype, "char string") == 0 ) { 
    status = add_aux_fld(tbl, fld, sz_opfile, "sz", &fld_id);
    cBYE(status);
  }
  if ( is_any_null ) {
    status = add_aux_fld(tbl, fld, nn_opfile, "nn", &fld_id);
    cBYE(status);
  }
 BYE:
  free_if_non_null(val);
  free_if_non_null(fldtype);
  free_if_non_null(op);
  free_if_non_null(start);
  free_if_non_null(incr);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(sz_opfile);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  fclose_if_non_null(sz_ofp);
  rs_munmap(X, nX);
  rs_munmap(sz_X, sz_nX);
  return(status);
}
