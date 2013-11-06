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
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "get_type_op_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"

#define MAX_PACK_FLDS 8

// START FUNC DECL
int
pack(
     char *tbl,
     char *str_flds,
     char *str_shifts,
     char *str_dst_fldtype,
     char *fout
     )
// STOP FUNC DECL
{
  int status = 0;

  char **Y = NULL; int  nY = 0;
  char **flds = NULL; int n_flds = 0;
	
  char *fout_X; size_t fout_nX = 0;

  char *Xs[MAX_PACK_FLDS]; size_t nXs[MAX_PACK_FLDS];
  int shifts[MAX_PACK_FLDS];
  int fld_ids[MAX_PACK_FLDS]; int nn_fld_ids[MAX_PACK_FLDS];
  FLD_REC_TYPE fld_recs[MAX_PACK_FLDS]; FLD_REC_TYPE nn_fld_recs[MAX_PACK_FLDS];
  FLD_TYPE dst_fldtype;
  char *endptr;
  TBL_REC_TYPE tbl_rec; int tbl_id; long long nR;
  char opfile[MAX_LEN_FILE_NAME+1]; int ddir_id;
  int fout_id; FLD_REC_TYPE fout_rec;

  // basic checks
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_shifts == NULL ) || ( *str_shifts == '\0' ) ) { go_BYE(-1); }
  if ( ( str_dst_fldtype == NULL ) || ( *str_dst_fldtype == '\0' ) ) { go_BYE(-1); }
  if ( ( fout == NULL ) || ( *fout == '\0' ) ) { go_BYE(-1); }
  status = unstr_fldtype(str_dst_fldtype, &dst_fldtype); cBYE(status);
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  nR = tbl_rec.nR;
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  // initializations
  for ( int i = 0; i < MAX_PACK_FLDS; i++ ) {
    Xs[i] = NULL; nXs[i] = 0;
    fld_ids[i] = -1; nn_fld_ids[i] = -1;
    shifts[i] = -1;
    zero_fld_rec(&(fld_recs[i])); zero_fld_rec(&(nn_fld_recs[i]));
  }
  // get names of fields to pack 
  status = explode(str_flds, ':', &flds, &n_flds); cBYE(status);
  if ( n_flds <= 1 ) { go_BYE(-1); }
  if ( n_flds > 8 ) { go_BYE(-1); }

  // get shifts for each field 
  status = explode(str_shifts, ':', &Y, &nY); cBYE(status);
  if ( nY != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    shifts[i] = strtoll(Y[i], &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( shifts[i] < 0 ) { go_BYE(-1); }
  }
  // get access to all inputs 
  for ( int i = 0; i < n_flds; i++ ) {
    status = is_fld(NULL, tbl_id, flds[i], &(fld_ids[i]), &(fld_recs[i]), 
		    &(nn_fld_ids[i]), &(nn_fld_recs[i]));
    cBYE(status);
    if ( fld_ids[i] < 0 ) { go_BYE(-1); }
    if ( nn_fld_ids[i] >= 0 ) { go_BYE(-1); }
    // check field type is okay
    switch ( fld_recs[i].fldtype ) { 
    case I1 : case I2 : case I4 : case I8 : break;
    default : go_BYE(-1); break;
    }
    status = get_data(fld_recs[i], &(Xs[i]), &(nXs[i]), false); cBYE(status);
  }
  // check output type 
  switch ( dst_fldtype ) { 
  case I1 : 
    for ( int i = 0; i < n_flds; i++ ) { 
      if ( shifts[i] >= 8 ) { go_BYE(-1); }
    }
    break;
  case I2 : 
    for ( int i = 0; i < n_flds; i++ ) { 
      if ( shifts[i] >= 16 ) { go_BYE(-1); }
    }
    break;
  case I4 : 
    for ( int i = 0; i < n_flds; i++ ) { 
      if ( shifts[i] >= 32 ) { go_BYE(-1); }
    }
    break;
  case I8 : 
    for ( int i = 0; i < n_flds; i++ ) { 
      if ( shifts[i] >= 64 ) { go_BYE(-1); }
    }
    break;
  default : go_BYE(-1); break;
  }
  // allocate space for output 
  int fldsz = -1;
  status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
  size_t filesz = fldsz * nR; 
  status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &fout_X, &fout_nX, true); cBYE(status);

  switch ( dst_fldtype ) {
  case I4 :
#include "incl_pack_I4.c"
    break;
  case I8 :
#include "incl_pack_I8.c"
    break;
  default :
    go_BYE(-1);
    break;
  }
  /* add fields */
  status = add_fld(tbl_id, fout, ddir_id, opfile, dst_fldtype, 
      -1, &fout_id, &fout_rec);
  cBYE(status);
 BYE:
  for ( int i = 0; i < n_flds; i++ ) {
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(Y[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(fout_X, fout_nX);
  free_if_non_null(Y);
  free_if_non_null(flds);
  return(status);
}
