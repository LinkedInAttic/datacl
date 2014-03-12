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
#include "regex_match.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

#define MAX_SHIFT 16 /* Maximum amount to shift by */
//---------------------------------------------------------------
// START FUNC DECL
int 
regex_match(
	    char *tbl,
	    char *f1,
	    char *regex,
	    char *matchtype,
	    char *f2
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec;
  long long nR; 
  int tbl_id = INT_MIN, f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int                   f2_id = INT_MIN;
  char opfile[MAX_LEN_FILE_NAME+1];
  int fldsz = INT_MAX, ddir_id = INT_MAX;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( regex == NULL ) || ( *regex == '\0' ) ) { go_BYE(-1); }
  if ( ( matchtype == NULL ) || ( *matchtype == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;

  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  if ( f1_rec.fldtype != clob ) { go_BYE(-1);}

  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  //---------------------------------------------
  // make space for output 
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  status = mk_temp_file(opfile, (nR * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, true); cBYE(status);
  //---------------------------------------------
  int rec_len = strlen(regex) + 1;
  if ( strcmp(matchtype, "exact") == 0 ) {
    for ( long long i = 0; i < nR; i++ ) { 
      // fprintf(stderr, "Comparing with %s \n", f1_X);
      if ( strcmp(f1_X, regex) == 0 ) {
	f2_X[i] = 1;
	f1_X += rec_len;
      }
      else {
	f2_X[i] = 0;
	char *cptr = f1_X;
	for ( ; *cptr != '\0'; cptr++ ) {
	}
	cptr++; // skip over null character
	f1_X = cptr;
      }
    }
  }
  else {
    go_BYE(-1); // TO BE IMPLEMENTED
  }
  //---------------------------------------
  rs_munmap(f2_X, f2_nX);
  status = add_fld(tbl_id, f2, ddir_id, opfile, I1, -1, &f2_id, &f2_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
