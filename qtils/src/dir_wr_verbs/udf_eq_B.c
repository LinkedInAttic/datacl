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
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "copy_fld.h"
#include "meta_globals.h"

#include "conv_I1_to_B.h"
#include "assign_I1.h"

#include "cmp_eq_scalar_I4.h"
#include "cmp_eq_scalar_I8.h"
#include "cmp_eq_scalar_F4.h"
#include "cmp_eq_scalar_I1.h"
#include "cmp_eq_scalar_I2.h"
#include "cmp_eq_mult_scalar_I1.h"
#include "cmp_eq_mult_scalar_I2.h"
#include "cmp_eq_mult_scalar_I4.h"
#include "cmp_eq_mult_scalar_I8.h"


//---------------------------------------------------------------
// START FUNC DECL
int 
udf_eq_B(
	 char *tbl,
	 char *f1,
	 char *str_scalar,
	 char *f2
	 )
// STOP FUNC DECL
{
  int status = 0;
  long long nR; int tbl_id;
  TBL_REC_TYPE tbl_rec;

  FLD_REC_TYPE f1_rec, nn_f1_rec; int f1_id, nn_f1_id;
  FLD_REC_TYPE f2_rec           ; int f2_id;
  char opfile[MAX_LEN_FILE_NAME+1];

  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;
  long long svals_I8[MAX_NUM_SCALARS];
  int       svals_I4[MAX_NUM_SCALARS];
  short     svals_I2[MAX_NUM_SCALARS];
  char      svals_I1[MAX_NUM_SCALARS];
  float     svals_F4[MAX_NUM_SCALARS];
  double    svals_F8[MAX_NUM_SCALARS];

  char *f1_X = NULL;   size_t f1_nX = 0;
  char *f2_X = NULL;   size_t f2_nX = 0;

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( str_scalar == NULL ) || ( *str_scalar == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } 

  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  //----------------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR; 

  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  int num_scalar_vals = -1;

  status =  break_into_scalars(str_scalar, f1_rec.fldtype, &num_scalar_vals,
			       &sval_I8, &sval_I4, &sval_I2, &sval_I1, &sval_F4, &sval_F8,
			       svals_I8, svals_I4, svals_I2, svals_I1, svals_F4, svals_F8);
  cBYE(status);
  /*-------------------------------------------- */
  // allocate space for output
  long long filesz = -1; int ddir_id = -1;
  status = get_file_size_B(nR, &filesz); cBYE(status);
  status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, true); cBYE(status);

#define BLK_SZ 4096
  if ( ( ( BLK_SZ / 8 ) * 8 )  != BLK_SZ )  { go_BYE(-1); }
  int nT;
  nT = nR / BLK_SZ; 
  if ( nT == 0 ) { nT = 1; } // Need at least one thread
  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = tid * BLK_SZ;
    long long ub = lb  + BLK_SZ;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = ub - lb;
    if ( nX > (2*BLK_SZ) ) { status = -1; continue; }
    char opI1[2*BLK_SZ]; /*  chunk of output, as character array */
    char opB[2*(BLK_SZ/8)]; /* chunk of output, as bit array,8 bits in a byte */
    char      *f1I1 = (char      *)f1_X; f1I1 += lb;
    short     *f1I2 = (short     *)f1_X; f1I2 += lb;
    int       *f1I4 = (int       *)f1_X; f1I4 += lb;
    long long *f1I8 = (long long *)f1_X; f1I8 += lb;
    // Initialize to all 0 
    assign_const_I1(opI1, 2*BLK_SZ, (char)0);

    switch ( f1_rec.fldtype ) { 
    case I1 : 
      if ( num_scalar_vals == 1 ) { 
	cmp_eq_scalar_I1(f1I1, sval_I1, nX, opI1);
      }
      else {
	cmp_eq_mult_scalar_I1(f1I1, nX, svals_I1, num_scalar_vals, opI1);
      }
      break;
    case I2 : 
      if ( num_scalar_vals == 1 ) { 
	cmp_eq_scalar_I2(f1I2, sval_I2, nX, opI1);
      }
      else {
	cmp_eq_mult_scalar_I2(f1I2, nX, svals_I2, num_scalar_vals, opI1);
      }
      break;
    case I4 : 
      if ( num_scalar_vals == 1 ) { 
	cmp_eq_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else {
	cmp_eq_mult_scalar_I4(f1I4, nX, svals_I4, num_scalar_vals, opI1);
      }
      break;
    case I8 : 
      if ( num_scalar_vals == 1 ) { 
	cmp_eq_scalar_I8(f1I8, sval_I8, nX, opI1);
      }
      else {
	cmp_eq_mult_scalar_I8(f1I8, nX, svals_I8, num_scalar_vals, opI1);
      }
      break;
    default :
      status = -1; continue;
      break;
    }
    /* convert opI1 to opB */
    status = conv_I1_to_B(opI1, nX, opB); if ( status < 0 ) { continue; }
    /* write opB to actual output */
    /* read this code carefully: TODO P2 */
    if ( tid == (nT-1) ) { /* last guy may have different sized block */
      if ( nX == BLK_SZ ) {
        memcpy(f2_X+(lb/8), opB, BLK_SZ/8);
      }
      else {
        memcpy(f2_X+(lb/8), opB, 2*BLK_SZ/8);
      }
    }
    else {
      memcpy(f2_X+(lb/8), opB, BLK_SZ/8);
    }
  }
  cBYE(status);


  /* add fields */
  status = add_fld(tbl_id, f2, ddir_id, opfile, B, -1, &f2_id, &f2_rec);
  cBYE(status);

 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
