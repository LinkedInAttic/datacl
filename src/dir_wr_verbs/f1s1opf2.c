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
#include "sort.h"
#include "get_nR.h"
#include "f1s1opf2.h"
#include "vec_f1s1opf2.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

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
  char *f1_X    = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *sz_f1_X = NULL; size_t sz_f1_nX = 0;
  long long nR; 
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE f1_rec, nn_f1_rec; int f1_id, nn_f1_id;
  FLD_REC_TYPE f2_rec, nn_f2_rec; int f2_id, nn_f2_id;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  FLD_TYPE f2_fldtype;
  int ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }

  for ( int i = 0; i < MAX_LEN_FILE_NAME+1; i++ ) { nn_opfile[i] = '\0'; } 
  for ( int i = 0; i < MAX_LEN_FILE_NAME+1; i++ ) { opfile[i] = '\0'; } 
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Get nn field for f1 if if it exists
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); 
    cBYE(status);
  } 
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
            ( strcmp(op, ">>") == 0 ) || 
           ( strcmp(op, "<||>") == 0 ) || 
	   ( strcmp(op, "<=||>=") == 0 ) || 
	   ( strcmp(op, ">&&<") == 0 ) || 
	   ( strcmp(op, ">=&&<=") == 0 ) 
	    ) {
	// all is well 
    status = vec_f1s1opf2(nR, f1_rec.fldtype, f1_X, nn_f1_X, 
	str_scalar, op, &ddir_id, opfile, &nn_ddir_id, nn_opfile, &f2_fldtype);
    cBYE(status);
    status = add_fld(tbl_id, f2, ddir_id, opfile, f2_fldtype, -1, &f2_id, &f2_rec);
    cBYE(status);
    if ( *nn_opfile != '\0' ) { /* there is a nn file */
      status = add_aux_fld(NULL, tbl_id, NULL, f2_id, nn_ddir_id, nn_opfile, 
	  "nn", &nn_f2_id, &nn_f2_rec);
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
  rs_munmap(sz_f1_X, sz_f1_nX);
  return(status);
}
