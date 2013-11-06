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
#include "auxil.h"
#include "mk_temp_file.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "add_fld.h"
#include "app_fld.h"
#include "get_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "drop_nn_fld.h"
#include "assign_I1.h"
#include "meta_globals.h"

extern char *g_data_dir;
extern char *g_cwd;
//---------------------------------------------------------------
// Appends data of field f2 in table T2 to field f1 in Table t1
// START FUNC DECL
int 
app_fld(
	char *t1,
	char *f1,
	char *t2,
	char *f2
	)
// STOP FUNC DECL
{
  int status = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *Y = NULL; size_t nY = 0;
  char *X2 = NULL; size_t nX2 = 0;
  TBL_REC_TYPE t1_rec, t2_rec; int t1_id, t2_id;
  FLD_REC_TYPE f1_rec, nn_f1_rec, f2_rec, nn_f2_rec;
  char nn_val;
  long long nR1, nR2; 
  int f1_id, nn_f1_id;
  int f2_id, nn_f2_id;
  char strbuf[32];
  FILE *ofp = NULL;
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int nn_ddir_id = INT_MAX;
  //----------------------------------------------------------------
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  nR1 = t1_rec.nR;
  chk_range(t1_id, 0, g_n_tbl);

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  nR2 = t2_rec.nR;
  chk_range(t2_id, 0, g_n_tbl);

  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = int_get_meta(t1_id, f1_id, "is_external", strbuf); cBYE(status);
  if ( strcmp(strbuf, "1") == 0 ) { go_BYE(-1); }

  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  status = int_get_meta(t2_id, f2_id, "is_external", strbuf); cBYE(status);
  if ( strcmp(strbuf, "1") == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Append X2 to field for f1
  chdir(g_data_dir);
  ofp = fopen(f1_rec.filename, "ab");
  return_if_fopen_failed(ofp, f1_rec.filename, "ab");
  status = rs_mmap(f2_rec.filename, &X2, &nX2, 0); cBYE(status);
  fwrite(X2, nX2, 1, ofp);
  rs_munmap(X2, nX2);
  fclose_if_non_null(ofp);
  //--------------------------------------------------------
  // See if there is an auxilary field (nn) to copy
  if ( ( nn_f1_id < 0 ) && ( nn_f2_id < 0 ) ) { 
    /* Nothing to do */
  }
  else if ( ( nn_f1_id < 0 ) && ( nn_f2_id >= 0 ) ) { 
    /* f1 does not have a nn field but f2 does */
    /* Create a file for newly created nn field for f1 */
    status = mk_temp_file(nn_opfile, ((nR1+nR2)*sizeof(char)), &nn_ddir_id); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_opfile, &nn_X, &nn_nX, true); cBYE(status);
    /* Set first nR1 values to true */
    assign_const_I1(nn_X, nR1, 1);
    /* Set next nR2 values based on f2 */
    status = rs_mmap(nn_f2_rec.filename, &Y, &nY, 0); cBYE(status);
    memcpy(nn_X+nR1, Y, nY);
    rs_munmap(Y, nY);
    rs_munmap(nn_X, nn_nX);
    /* Add to meta data */
    status = add_aux_fld(NULL, t1_id, NULL, f1_id, nn_ddir_id, nn_opfile, "nn", 
	&nn_f1_id, &nn_f1_rec);
    cBYE(status);
  }
  else if ( ( nn_f1_id >= 0 ) && ( nn_f2_id < 0 ) ) { 
    /* f1 has a nn field but f2 does not */
    /* Create a file for newly created nn field for f2 */
    ofp = fopen(nn_f1_rec.filename, "ab"); 
    /* Set next nR2 values to true */
    nn_val = TRUE;
    for ( long long i = 0; i < nR2; i++ ) {  // TODO: SPEED THIS UP
      fwrite(&nn_val, sizeof(char), 1, ofp);
    }
    fclose_if_non_null(ofp);
    /* Add to meta data */
    status = drop_nn_fld(t1, f1); cBYE(status); 
    status = add_aux_fld(NULL, t1_id, NULL, f1_id, f1_rec.ddir_id, 
	nn_opfile, "nn", 
	&nn_f1_id, &nn_f1_rec);
    cBYE(status);
    // done in add_aux_fld g_fld[f1_id].nn_fld_id = nn_f1_id;

  }
  else { /* Both f1 and f2 have nn fields */
    char *file1 = nn_f1_rec.filename;
    ofp = fopen(file1, "ab");
    return_if_fopen_failed(ofp, file1, "ab");
    status = q_mmap(nn_f2_rec.ddir_id, nn_f2_rec.filename, &Y, &nY, 0); cBYE(status);
    fwrite(Y, nY, 1, ofp);
    fclose_if_non_null(ofp);
  }
  //--------------------------------------------------------
 BYE:
  fclose_if_non_null(ofp);
  return(status);
}
