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
#include "open_temp_file.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "f1opf2_cum.h"
#include "f1opf2_shift.h"
#include "f1opf2_hash.h"
#include "vec_f1opf2.h"
#include "aux_fld_meta.h"
#include "str_xform.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;
#define MAX_SHIFT 16 /* Maximum amount to shift by */
//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2(
       char *tbl,
       char *f1,
       char *str_op_spec,
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
  int tbl_id = INT_MIN, f1_id = INT_MIN, nn_f1_id = INT_MIN, sz_f1_id = INT_MIN;
  int         f2_id = INT_MIN, nn_f2_id = INT_MIN, sz_f2_id = INT_MIN;
  char str_meta_data[1024];
  int f1type;
  char *op = NULL, *str_val = NULL;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  FILE *sz_ofp = NULL; char *sz_opfile = NULL;
  char *xform_enum = NULL, *hash_algo = NULL;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = extract_name_value(str_op_spec, "op=", ":", &op);
  cBYE(status);
  if ( op == NULL ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = chk_if_ephemeral(&f2); cBYE(status);
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); 
  cBYE(status);
  // Get nn field for f1 if if it exists
  nn_f1_id = g_fld[f1_id].nn_fld_id;
  if ( nn_f1_id >= 0 ) { 
    nn_f1_meta = &(g_fld[nn_f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nn_f1_X, &nn_f1_nX, 0); 
    cBYE(status);
  }
  //---------------------------------------------
  status = mk_ifldtype(f1_meta->fldtype, &f1type);
  /* TODO: Why do we get an error for 'char string' ? */
  // If f1 type is 'char string', then get sz field 
  if ( strcmp(f1_meta->fldtype, "char string") == 0 ) {
    sz_f1_id = g_fld[f1_id].sz_fld_id;
    chk_range(sz_f1_id, 0, g_n_fld);
    sz_f1_meta = &(g_fld[sz_f1_id]);
    status = rs_mmap(sz_f1_meta->filename, &sz_f1_X, &sz_f1_nX, 0); 
    cBYE(status);
  }
  //--------------------------------------------------------
  if ( strcmp(op, "xform") == 0 ) {
    bool is_some_null;
    status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
    status = open_temp_file(&nn_ofp, &nn_opfile, 0); cBYE(status);
    status = open_temp_file(&sz_ofp, &sz_opfile, 0); cBYE(status);
    status = str_xform(nR, f1_X, (int *)sz_f1_X, nn_f1_X, str_op_spec,
	ofp, nn_ofp, sz_ofp, &is_some_null);
    cBYE(status);
    fclose_if_non_null(ofp);
    fclose_if_non_null(nn_ofp);
    fclose_if_non_null(sz_ofp);
    sprintf(str_meta_data, "filename=%s:fldtype=char string:n_sizeof=0", 
	opfile);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
    status = add_aux_fld(tbl, f2, sz_opfile, "sz", &sz_f2_id);
    cBYE(status);
    if ( is_some_null ) { 
      status = add_aux_fld(tbl, f2, nn_opfile, "nn", &nn_f2_id);
      cBYE(status);
    }
    else {
      unlink(nn_opfile);
    }
  }
  else if ( strcmp(op, "hash") == 0 ) {
    int hash_len; char *endptr = NULL;

    status = extract_name_value(str_op_spec, "len=", ":", &str_val);
    cBYE(status);
    if  ( str_val == NULL ) { go_BYE(-1); }
    hash_len = strtol(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }

    status = extract_name_value(str_op_spec, "hash_algo=", ":", &hash_algo);
    cBYE(status);

    status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
    status = f1opf2_hash(hash_algo, nR, f1_X, f1_nX, (int *)sz_f1_X, 
	f1_meta->n_sizeof, nn_f1_X, hash_len, ofp);
    cBYE(status);
    fclose_if_non_null(ofp);
    sprintf(str_meta_data, "filename=%s", opfile);
    switch ( hash_len ) { 
      case 4 : strcat(str_meta_data, ":fldtype=int:n_sizeof=4"); break;
      case 8 : strcat(str_meta_data, ":fldtype=long long:n_sizeof=8"); break;
      default : go_BYE(-1); break;
    }
    fclose_if_non_null(ofp);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
  }
  else if ( ( strcmp(op, "conv") == 0 ) || 
            ( strcmp(op, "dateconv" ) == 0 ) || 
            ( strcmp(op, "bitcount" ) == 0 ) || 
            ( strcmp(op, "sqrt" ) == 0 ) || 
            ( strcmp(op, "!" ) == 0 ) || 
            ( strcmp(op, "++" ) == 0 ) || 
            ( strcmp(op, "--" ) == 0 ) || 
            ( strcmp(op, "~" ) == 0 )
	    ) {
    status = vec_f1opf2(nR, f1type, str_meta_data,
	f1_X, nn_f1_X, sz_f1_X, op, str_op_spec, &opfile, &nn_opfile);
    cBYE(status);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
    if ( nn_opfile != NULL ) { 
      status = add_aux_fld(tbl, f2, nn_opfile, "nn", &nn_f2_id);
      cBYE(status);
    }
  }
  else if ( strcmp(op, "shift") == 0 ) {
    int ival; char *endptr = NULL;
    status = extract_name_value(str_op_spec, "val=", ":", &str_val);
    cBYE(status);
    ival = strtol(str_val, &endptr, 10);
    if ( *endptr != '\0' ) { 
      fprintf(stderr, "invalid shift specifier = [%s] \n", str_val);
      go_BYE(-1); 
    }
    if ( ( ival == 0 ) || ( ival > MAX_SHIFT ) || 
	(  ival < -MAX_SHIFT ) || (ival >= nR )) {
      fprintf(stderr, "shift = [%d] is out of bounds \n", ival);
      go_BYE(-1);
    }

    status = open_temp_file(&ofp, &opfile, 0);
    cBYE(status);
    status = open_temp_file(&nn_ofp, &nn_opfile, 0);
    cBYE(status);
    status = f1opf2_shift(f1_X, nn_f1_X, nR, f1_meta->fldtype, ival, 
	  ofp, nn_ofp);
    cBYE(status);
    fclose_if_non_null(ofp);
    fclose_if_non_null(nn_ofp);
    sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",opfile,
	f1_meta->fldtype, f1_meta->n_sizeof);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
    status = add_aux_fld(tbl, f2, nn_opfile, "nn", &nn_f2_id);
    cBYE(status);
  }
  else if ( strcmp(op, "cum") == 0 ) {
    /* TODO: Document. If you do a cum, resultant field is all def */
    char *new_fld_type = NULL; int optype, n_sizeof;
    char *str_reset_on = NULL, *str_reset_to = NULL, *endptr = NULL;
    long long reset_on = 0, reset_to = 0; bool is_reset = false;
    status = extract_name_value(str_op_spec, "newtype=", ":", &new_fld_type);
    cBYE(status);
    status = extract_name_value(str_op_spec, "reset_on=", ":",
	&str_reset_on);
    cBYE(status);
    if ( str_reset_on != NULL ) {
      reset_on = strtoll(str_reset_on, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      is_reset = true;
      status = extract_name_value(str_op_spec, "reset_to=", ":",
	&str_reset_to);
      cBYE(status);
      if ( str_reset_to == NULL ) { go_BYE(-1); }
      reset_to = strtoll(str_reset_to, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
    }
    if ( new_fld_type == NULL ) { 
      optype = f1type;
      new_fld_type = strdup(f1_meta->fldtype);
      n_sizeof = f1_meta->n_sizeof;
    }
    else {
      if ( strcmp(new_fld_type, "int") == 0 ) { 
	optype = FLDTYPE_INT;
	n_sizeof = sizeof(int);
      }
      else if ( strcmp(new_fld_type, "long long") == 0 ) { 
	optype = FLDTYPE_LONGLONG;
	n_sizeof = sizeof(long long);
      }
      else { 
	go_BYE(-1);
      }
    }
    status = f1opf2_cum(f1_X, nR, f1type, optype, is_reset,
	reset_on, reset_to, &opfile);
    cBYE(status);
    sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",opfile,
	new_fld_type, n_sizeof);
    status = add_fld(tbl, f2, str_meta_data, &f2_id);
    cBYE(status);
    free_if_non_null(new_fld_type);
    free_if_non_null(str_reset_on);
    free_if_non_null(str_reset_to);
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
BYE:
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(sz_f1_X, sz_f1_nX);
  free_if_non_null(op);
  free_if_non_null(xform_enum);
  free_if_non_null(hash_algo);
  free_if_non_null(str_val);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(sz_opfile);
  return(status);
}
