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
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "ext_f_to_s.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
bindmp(
       char *tbl,
       char *str_flds,
       char *cfld,
       char *opfile
       )
// STOP FUNC DECL
{
  int status = 0;
#define MAX_NUM_FLDS_TO_DMP 16
  int fld_size[MAX_NUM_FLDS_TO_DMP];
  char *Xs[MAX_NUM_FLDS_TO_DMP]; size_t nXs[MAX_NUM_FLDS_TO_DMP];
  char *X = NULL;  size_t nX = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_recs[MAX_NUM_FLDS_TO_DMP];
  FLD_REC_TYPE nn_fld_rec, cfld_rec, nn_cfld_rec;
  char **flds = NULL;  int n_flds = -1;
  long long nR, nn_nR; 
  int tbl_id = INT_MIN; 
  int fld_id = INT_MIN, cfld_id = INT_MIN;
  int nn_fld_id = INT_MIN, nn_cfld_id = INT_MIN;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  int rec_size;
  char cwd[MAX_LEN_DIR_NAME+1];


  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( opfile == NULL ) || ( *opfile == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  if ( n_flds > MAX_NUM_FLDS_TO_DMP ) { go_BYE(-1); }
  //--------------------------------------------------------
  for ( int i = 0; i < MAX_NUM_FLDS_TO_DMP; i++ ) { 
    zero_fld_rec(fld_recs+i);
    Xs[i] = NULL;
    nXs[i] = 0;
  }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  /* If there is a condition field for the export, get a handle on it */
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) )  {
    status = is_fld(NULL, tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); cBYE(status);
    chk_range(cfld_id, 0, g_n_fld);
    if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0);
    cBYE(status);
  }
  //--------------------------------------------------------
  /* Get meta data for all fields to export. Also, get record size */
  rec_size = 0;
  for ( int i = 0; i < n_flds; i++ ) {
    int n_sizeof;
    status = is_fld(NULL, tbl_id, flds[i], &fld_id, &(fld_recs[i]),
		    &nn_fld_id, &nn_fld_rec);
    cBYE(status);
    chk_range(fld_id, 0, g_n_fld);
    // bindmp can be done only when no null values 
    if ( nn_fld_id >= 0 ) { go_BYE(-1); } 
    status = get_data(fld_recs[i], &(Xs[i]), &(nXs[i]), 0);
    cBYE(status);
    status = get_fld_sz(fld_recs[i].fldtype, &n_sizeof); cBYE(status);
    rec_size += n_sizeof;
    fld_size[i] = n_sizeof;
  } 
  /* allocate space for output */
  if ( cfld_X == NULL ) { 
    nn_nR = nR;
  }
  else {
    char *endptr; 
    char str_rslt[32];
    zero_string(str_rslt, 32);
    status = ext_f_to_s(tbl, cfld, "sum", str_rslt, 32); cBYE(status);
    // truncate string after colon
    for ( int i = 0; i < 32; i++ ) { 
      if ( str_rslt[i] == ':' ) { str_rslt[i] = '\0'; }
    }
    nn_nR = strtol(str_rslt, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( nn_nR == 0 ) {
      fprintf(stderr, "WARNING! Nothing to dump\n"); goto BYE;
    }
  }
  zero_string(cwd, (MAX_LEN_DIR_NAME+1));
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  status = mk_file(cwd, cwd, opfile, (rec_size * nn_nR)); cBYE(status);
  status = rs_mmap(opfile, &X, &nX, 1); cBYE(status);
  //------------------------
  if ( cfld_id < 0 ) { /* no condition field */
    for ( long long i = 0; i < nR; i++ ) { 
      for ( int j = 0; j < n_flds; j++ ) {
        int fldsz = fld_size[j];
        memcpy(X, Xs[j], fldsz);
        X += fldsz;
        Xs[j] += fldsz;
      }
    }
  }
  else {
    long long chk_nR = 0;
    for ( long long i = 0; i < nR; i++ ) {
      /* Skip this row if condition field asks you to do so */
      if ( ( cfld_X == NULL ) || ( cfld_X[i] == TRUE ) ) {
	chk_nR++;
	for ( int j = 0; j < n_flds; j++ ) {
	  int fldsz = fld_size[j];
	  memcpy(X, Xs[j], fldsz);
	  X += fldsz;
	  Xs[j] += fldsz;
	}
      }
      else { // Skipping row. Just advance pointers
	for ( int j = 0; j < n_flds; j++ ) {
	  int fldsz = fld_size[j];
	  Xs[j] += fldsz;
	}
      }
    }
    if ( chk_nR != nn_nR ) { go_BYE(-1); }
  }
 BYE:
  rs_munmap(X, nX);
  for ( int i = 0; i < n_flds; i++ ) { 
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(cfld_X, cfld_nX);
  free_if_non_null(flds);
  return(status);
}
