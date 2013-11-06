/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mk_temp_file.h"
#include "get_nR.h"
#include "f1opf2_cum.h"
#include "f1opf2_shift.h"
#include "vec_f1opf2.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "extract_S.h"
#include "extract_I4.h"
#include "smear.h"
#include "meta_globals.h"

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
  char *op_X = NULL; size_t op_nX = 0;
  char *nn_op_X = NULL; size_t nn_op_nX = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  long long nR; 
  int tbl_id = INT_MIN, f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int                   f2_id = INT_MIN, nn_f2_id = INT_MIN;
  FLD_TYPE f1type, f2type;
#define BUFLEN 32
  char op[BUFLEN]; 
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int fldsz = INT_MAX, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_op_spec == NULL ) || ( *str_op_spec == '\0' ) ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  bool is_null;
  status = extract_S(str_op_spec, "op=[", "]", op, BUFLEN, &is_null);
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1type = f1_rec.fldtype;
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Get nn field for f1 if if it exists
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  //---------------------------------------------
  if ( ( strcmp(op, "conv") == 0 ) || 
            ( strcmp(op, "bitcount" ) == 0 ) || 
            ( strcmp(op, "sqrt" ) == 0 ) || 
            ( strcmp(op, "abs" ) == 0 ) || 
            ( strcmp(op, "normal_cdf_inverse" ) == 0 ) || 
            ( strcmp(op, "reciprocal" ) == 0 ) || 
            ( strcmp(op, "!" ) == 0 ) || 
            ( strcmp(op, "++" ) == 0 ) || 
            ( strcmp(op, "--" ) == 0 ) || 
            ( strcmp(op, "~" ) == 0 )
	    ) {
    status = vec_f1opf2(nR, f1type, f1_X, nn_f1_X, op, 
	str_op_spec, &ddir_id, opfile, &nn_ddir_id, nn_opfile, &f2type);
    cBYE(status);
    status = add_fld(tbl_id, f2, ddir_id, opfile, f2type, -1, &f2_id, &f2_rec);
    cBYE(status);
    if ( strlen(nn_opfile) > 0 ) {
      status = add_aux_fld(NULL, tbl_id, NULL, f1_id, nn_ddir_id, 
	  nn_opfile, "nn", &nn_f2_id, &nn_f2_rec);
      cBYE(status);
    }
  }
  else if ( strcmp(op, "cum") == 0 ) {
    /* TODO: Document. If you do a cum, resultant field is all def */
    bool is_null;
    char rslt_buf[BUFLEN];
    status = extract_S(str_op_spec, "newtype=[", "]", 
	rslt_buf, BUFLEN, &is_null); cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    status = unstr_fldtype(rslt_buf, &f2type); cBYE(status);
    status = f1opf2_cum(f1_X, nR, f1type, f2type, &ddir_id, opfile); cBYE(status);
    status = add_fld(tbl_id, f2, ddir_id, opfile, f2type, -1, &f2_id, &f2_rec);
    cBYE(status);
  }
  else if ( strcmp(op, "smear") == 0 ) {
    bool is_plus_null, is_minus_null;
    int plus = -1, minus = -1; 
    status = extract_I4(str_op_spec, "plus=[", "]", &plus, &is_plus_null);
    cBYE(status);
    if ( is_plus_null ) { plus = 0; } else { if ( plus < 0 ) { go_BYE(-1);}}

    status = extract_I4(str_op_spec, "minus=[", "]", &minus, &is_minus_null);
    cBYE(status);
    if ( is_minus_null ) { minus = 0; } else { if ( minus < 0 ) { go_BYE(-1);}}

    if ( ( is_plus_null ) && ( is_minus_null ) ) { go_BYE(-1); }

    FLD_TYPE dst_fldtype = I1;
    status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR * fldsz), &ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &op_X, &op_nX, true); cBYE(status);
    status = smear(f1_X, nR, plus, minus, op_X); cBYE(status);
    rs_munmap(op_X, op_nX);
    status = add_fld(tbl_id, f2, ddir_id, opfile, f1_rec.fldtype, -1, 
	&f2_id, &f2_rec);
    cBYE(status);
  }
  else if ( strcmp(op, "shift") == 0 ) {
    int shift_by = 0;
    status = extract_I4(str_op_spec, "val=[", "]", &shift_by, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    if ( shift_by == 0 ) { go_BYE(-1); }
    status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR * fldsz), &ddir_id); cBYE(status);
    status = mk_temp_file(nn_opfile, (nR * sizeof(char)), &nn_ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &op_X, &op_nX, true); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_opfile, &nn_op_X, &nn_op_nX, true); cBYE(status);
    status = f1opf2_shift(f1_X, nn_f1_X, nR, f1_rec.fldtype, shift_by, 
	op_X, nn_op_X);
    cBYE(status);

    rs_munmap(op_X, op_nX);
    rs_munmap(nn_op_X, nn_op_nX);
    status = add_fld(tbl_id, f2, ddir_id, opfile, f1_rec.fldtype, -1, 
	&f2_id, &f2_rec);
    cBYE(status);
    status = add_aux_fld(NULL, tbl_id, f2, f2_id, nn_ddir_id, nn_opfile, 
	"nn", &nn_f2_id, &nn_f2_rec);
    cBYE(status);
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
BYE:
  rs_munmap(op_X, op_nX);
  rs_munmap(nn_op_X, nn_op_nX);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  return(status);
}
