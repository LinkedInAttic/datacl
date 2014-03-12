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
#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "ext_s_to_f.h"
#include "par_s_to_f.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
ext_s_to_f(
       const char *tbl,
       const char *fld,
       const char *str_scalar
       )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  FILE  *ofp = NULL; char opfile[MAX_LEN_FILE_NAME+1];
#define MAX_LEN 32
  char str_fldtype[MAX_LEN];
  char op[MAX_LEN];
  long long nR;
  int fldsz  = INT_MIN;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_TYPE fldtype;
  bool is_null = true;
  int ddir_id;
  //----------------------------------------------------------------
  zero_string(str_fldtype, MAX_LEN);
  zero_string(opfile, MAX_LEN_FILE_NAME);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  zero_string(op, MAX_LEN);
  status = extract_S(str_scalar, "op=[", "]", op, MAX_LEN, &is_null); 
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }

  status = extract_S(str_scalar, "fldtype=[", "]", str_fldtype, MAX_LEN, &is_null); 
  if ( is_null ) { go_BYE(-1); }
  if ( ( str_fldtype == NULL ) || ( *str_fldtype == '\0' ) ) { go_BYE(-1); }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);
  status = get_fld_sz(fldtype, &fldsz); cBYE(status); 
  //--------------------------------------------------------
  // Create storage 
  long long filesz = nR * fldsz;
  status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &X, &nX, 1); cBYE(status);
  //--------------------------------------------------------
  status = par_s_to_f(X, 0, nR, str_scalar); cBYE(status);
  rs_munmap(X, nX);
  // Update meta data 
  status = add_fld(tbl_id, fld, ddir_id, opfile, fldtype, -1, &fld_id, &fld_rec);
  cBYE(status);
 BYE:
  rs_munmap(X, nX);
  fclose_if_non_null(ofp);
  return(status);
}
