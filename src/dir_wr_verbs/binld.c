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
#include "auxil.h"
#include "dbauxil.h"
#include "open_temp_file.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "meta_globals.h"

#define MAX_NUM_FLDS_TO_LOAD 32
//---------------------------------------------------------------
// START FUNC DECL
int 
binld(
       char *tbl,
       char *str_flds,
       char *str_flds_spec,
       char *infile
       )
// STOP FUNC DECL
{
  int status = 0;
  char **flds = NULL;  char **spec = NULL;
  int rec_size = -1, n_flds = INT_MAX, chk_n_flds = INT_MIN;
  TBL_REC_TYPE tbl_rec; FLD_REC_TYPE fld_rec;
  int tbl_id = INT_MIN, fld_id = INT_MIN;
  long long nR;
  FILE *ifp = NULL;
  char  *X  = NULL; size_t  nX  = 0;
  char str_nR[32]; 

  char *Xs[MAX_NUM_FLDS_TO_LOAD]; size_t nXs[MAX_NUM_FLDS_TO_LOAD];
  int fldtype[MAX_NUM_FLDS_TO_LOAD];
  int fldsz[MAX_NUM_FLDS_TO_LOAD];
  int ddir_id[MAX_NUM_FLDS_TO_LOAD];
  char opfiles[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_FILE_NAME+1];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds_spec == NULL ) || ( *str_flds_spec == '\0' ) ) { go_BYE(-1); }
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  zero_string(str_nR, 32);
  for ( int i = 0; i < MAX_NUM_FLDS_TO_LOAD; i++ ) { 
    ddir_id[i] = INT_MAX;
    Xs[i] = NULL;
    nXs[i] = 0;
    zero_string(opfiles[i], MAX_NUM_FLDS_TO_LOAD);
    fldtype[i] = -1;
    fldsz[i] = INT_MAX;
  }
  //--------------------------------------------------------
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  status = explode(str_flds, ':', &flds, &n_flds);
  cBYE(status);
  if ( n_flds < 1                    ) { go_BYE(-1); }
  if ( n_flds > MAX_NUM_FLDS_TO_LOAD ) { go_BYE(-1); }
  //--------------------------------------------------------
  for ( int i = 0; i < n_flds; i++ ) { 
    status = chk_fld_name(flds[i], 0); cBYE(status);
    for ( int j = i+1; j < n_flds; j++ ) { 
      if ( strcmp(flds[i], flds[j]) == 0 ) { 
	fprintf(stderr, "Duplicate field names not allowed \n");
	go_BYE(-1);
      }
    }
  }
  //--------------------------------------------------------
  if ( ( str_flds_spec == NULL ) || ( *str_flds_spec == '\0' ) ) { go_BYE(-1); }
  status = explode(str_flds_spec, ':', &spec, &chk_n_flds);
  cBYE(status);
  if ( chk_n_flds != n_flds ) { go_BYE(-1); }
  //--------------------------------------------------------
  rec_size = 0;
  for ( int j = 0; j < n_flds; j++ ) { 
    if  ( strcmp(spec[j], "I1") == 0 ) { 
      fldtype[j] = I1;
      rec_size  += sizeof(char);
      fldsz[j]   = sizeof(char);
    }
    else if  ( strcmp(spec[j], "I2") == 0 ) { 
      fldtype[j] = I2;
      rec_size  += sizeof(short);
      fldsz[j]   = sizeof(short);
    }
    else if  ( strcmp(spec[j], "I4") == 0 ) { 
      fldtype[j] = I4;
      rec_size  += sizeof(int);
      fldsz[j]   = sizeof(int);
    }
    else if  ( strcmp(spec[j], "I8") == 0 ) {
      fldtype[j] = I8;
      rec_size += sizeof(long long);
      fldsz[j]   = sizeof(long long);
    }
    else if  ( strcmp(spec[j], "F4") == 0 ) { 
      fldtype[j] = F4;
      rec_size += sizeof(float);
      fldsz[j]   = sizeof(float);
    }
    else if  ( strcmp(spec[j], "F8") == 0 ) { 
      fldtype[j] = F8;
      rec_size += sizeof(double);
      fldsz[j]   = sizeof(double);
    }
    else {
      go_BYE(-1);
    }
  }
  /* mmap input file and get number of rows */
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  nR = nX / rec_size;
  if ( nR * rec_size != nX ) { go_BYE(-1); }
  sprintf(str_nR, "%lld", nR);
  /* Create output files for each field */
  for ( int j = 0; j < n_flds; j++ ) { 
    status = mk_temp_file(opfiles[j], (fldsz[j] * nR), &(ddir_id[j])); cBYE(status);
  }
  for ( int j = 0; j < n_flds; j++ ) { 
    status = q_mmap(ddir_id[j], opfiles[j], &(Xs[j]), &(nXs[j]), 1); cBYE(status);
  }
  //--------------------------------------------------------
  for ( long long i = 0; i < nR; i++ ) { 
    for ( int j = 0; j < n_flds; j++ ) { 
      memcpy(Xs[j], X, fldsz[j]);
      Xs[j] += fldsz[j];
      X     += fldsz[j];
    }
  }
  for ( int j = 0; j < n_flds; j++ ) { 
    rs_munmap(Xs[j], nXs[j]);
  }
  // Delete table if it exists 
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id >= 0 ) { 
    status = del_tbl(NULL, tbl_id);
    cBYE(status);
  }
  // Create an empty table
  status = add_tbl(tbl, str_nR, &tbl_id, &tbl_rec); cBYE(status);
  // Add fields to table 
  for ( int j = 0; j < n_flds; j++ ) { 
    status = add_fld(tbl_id, flds[j], ddir_id[j], opfiles[j], fldtype[j], 
	-1, &fld_id, &fld_rec);
    cBYE(status);
  }
  fclose_if_non_null(ifp);
BYE:
  fclose_if_non_null(ifp);
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( spec != NULL ) { free_if_non_null(spec[i]); }
    if ( flds != NULL ) { free_if_non_null(flds[i]); }
  }
  free_if_non_null(spec);
  free_if_non_null(flds);
  return(status);
}
