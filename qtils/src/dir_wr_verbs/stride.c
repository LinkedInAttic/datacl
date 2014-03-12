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
#include <values.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "set_meta.h"
#include "is_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

// START FUNC DECL
int
int_stride_I4(
    int *f1,
    long long nR1,
    long long start, 
    long long stride, 
    int *f2,
    char *nn_f2_X, 
    long long nR2,
    bool *ptr_is_any_null
    )
// STOP FUNC DECL
{
  int status = 0;
  *ptr_is_any_null = false;
  for ( int i = 0; i < nR2; i++ ) {
    long long f1_idx = start + i*stride;
    if ( ( f1_idx < 0 ) || ( f1_idx >= nR1 ) )  {
      f2[i] = 0;
      nn_f2_X[i] = FALSE;
      *ptr_is_any_null = true;
    }
    else {
      f2[i] = f1[f1_idx];
      nn_f2_X[i] = TRUE;
    }
  }
  return(status);
}

//---------------------------------------------------------------

// START FUNC DECL
int
int_stride_I8(
    long long *f1,
    long long nR1,
    long long start, 
    long long stride, 
    long long *f2,
    char *nn_f2_X, 
    long long nR2,
    bool *ptr_is_any_null
    )
// STOP FUNC DECL
{
  int status = 0;
  *ptr_is_any_null = false;
  for ( int i = 0; i < nR2; i++ ) {
    long long f1_idx = start + i*stride;
    if ( ( f1_idx < 0 ) || ( f1_idx >= nR1 ) )  {
      f2[i] = 0;
      nn_f2_X[i] = FALSE;
      *ptr_is_any_null = true;
    }
    else {
      f2[i] = f1[f1_idx];
      nn_f2_X[i] = TRUE;
    }
  }
  return(status);
}

//---------------------------------------------------------------
// START FUNC DECL
int 
ext_stride(
	   const char *t1,
	   const char *f1,
	   char *str_start,
	   char *str_stride,
	   const char *t2,
	   const char *f2,
	   char *str_nR2
    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
#define MAX_LEN 32
  long long nR1, nR2;
  TBL_REC_TYPE t1_rec; int t1_id; 
  TBL_REC_TYPE t2_rec; int t2_id; 
  FLD_REC_TYPE f1_rec; int f1_id; 
  FLD_REC_TYPE nn_f1_rec; int nn_f1_id; 
  FLD_REC_TYPE f2_rec; int f2_id; 
  FLD_REC_TYPE nn_f2_rec; int nn_f2_id; 
  long long start, stride; char *endptr;
  bool is_any_null;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int fldsz = INT_MAX, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;

  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  start = strtoll(str_start, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  stride = strtoll(str_stride, &endptr, 10); 
  if ( *endptr != '\0' ) { go_BYE(-1); }
  nR2 = strtoll(str_nR2, &endptr, 10); 
  if ( *endptr !=  '\0' ) { go_BYE(-1); }
  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  zero_string(nn_opfile, MAX_LEN_FILE_NAME+1);
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  nR1 = t1_rec.nR;
  status = is_fld(t1, -1, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  status = del_tbl(t2, -1); cBYE(status); 

  if ( ( start  <  0 ) || ( start >= nR1 ) ) { go_BYE(-1); }
  if ( ( stride <= 0 ) || ( stride >= nR1 ) ) { go_BYE(-1); }

  // allocate space for output
  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);

  status = mk_temp_file(opfile, (nR2 * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, true); cBYE(status);

  status = mk_temp_file(nn_opfile, (nR2 * sizeof(char)), &nn_ddir_id); cBYE(status);
  if ( start + (stride * nR2) >= nR2 ) { /* nn needed */
    status = q_mmap(nn_ddir_id, nn_opfile, &nn_f2_X, &nn_f2_nX, true); cBYE(status);
  }

  switch ( f1_rec.fldtype ) { 
    case I4 : 
      status = int_stride_I4((int *)f1_X, nR1, start, stride, 
	  (int *)f2_X, nn_f2_X, nR2, &is_any_null);
      break;
    case I8 : 
      status = int_stride_I8((long long *)f1_X, nR1, start, stride, 
	  (long long *)f2_X, nn_f2_X, nR2, &is_any_null);
      break;
    default :
      go_BYE(-1);
      break;
  }
  cBYE(status);

  status = add_tbl(t2, str_nR2, &t2_id, &t2_rec); cBYE(status);
  status = add_fld(t2_id, f2, ddir_id, opfile, f1_rec.fldtype, -1, &f2_id, &f2_rec);
  cBYE(status);
  if ( is_any_null == true ) { 
    status = add_aux_fld(NULL, t2_id, NULL, f2_id, nn_ddir_id, 
	nn_opfile, "nn", &nn_f2_id, &nn_f2_rec);
  }
  else {
  }

 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  return(status);
}
