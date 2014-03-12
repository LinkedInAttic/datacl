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
#include <math.h>
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
#include "unpack.h"

#define MAX_PACK_FLDS 8

int
get_mask_I4(
	    int in,
	    unsigned long long *ptr_out
	    )
{
  int status = 0;
  switch ( in ) { 
  case  1 : *ptr_out = 0x00000001; break;
  case  2 : *ptr_out = 0x00000003; break;
  case  3 : *ptr_out = 0x00000007; break;
  case  4 : *ptr_out = 0x0000000F; break;
  case  5 : *ptr_out = 0x0000001F; break;
  case  6 : *ptr_out = 0x0000003F; break;
  case  7 : *ptr_out = 0x0000007F; break;
  case  8 : *ptr_out = 0x000000FF; break;
  case  9 : *ptr_out = 0x000001FF; break;
  case 10 : *ptr_out = 0x000003FF; break;
  case 11 : *ptr_out = 0x000007FF; break;
  case 12 : *ptr_out = 0x00000FFF; break;
  case 13 : *ptr_out = 0x00001FFF; break;
  case 14 : *ptr_out = 0x00003FFF; break;
  case 15 : *ptr_out = 0x00007FFF; break;
  case 16 : *ptr_out = 0x0000FFFF; break;
  case 17 : *ptr_out = 0x0001FFFF; break;
  case 18 : *ptr_out = 0x0003FFFF; break;
  case 19 : *ptr_out = 0x0007FFFF; break;
  case 20 : *ptr_out = 0x000FFFFF; break;
  case 21 : *ptr_out = 0x001FFFFF; break;
  case 22 : *ptr_out = 0x003FFFFF; break;
  case 23 : *ptr_out = 0x007FFFFF; break;
  case 24 : *ptr_out = 0x00FFFFFF; break;
  case 25 : *ptr_out = 0x01FFFFFF; break;
  case 26 : *ptr_out = 0x03FFFFFF; break;
  case 27 : *ptr_out = 0x07FFFFFF; break;
  case 28 : *ptr_out = 0x0FFFFFFF; break;
  case 29 : *ptr_out = 0x1FFFFFFF; break;
  case 30 : *ptr_out = 0x3FFFFFFF; break;
  case 31 : *ptr_out = 0x7FFFFFFF; break;
  case 32 : *ptr_out = 0xFFFFFFFF; break;
  default : go_BYE(-1); break; 
  }
 BYE:
  return(status);
}

int
get_mask_I8(
	    int in,
	    unsigned long long *ptr_out
	    )
{
  int status = 0;
  switch ( in ) { 
  case  1 : *ptr_out = 0x0000000000000001; break;
  case  2 : *ptr_out = 0x0000000000000003; break;
  case  3 : *ptr_out = 0x0000000000000007; break;
  case  4 : *ptr_out = 0x000000000000000F; break;
  case  5 : *ptr_out = 0x000000000000001F; break;
  case  6 : *ptr_out = 0x000000000000003F; break;
  case  7 : *ptr_out = 0x000000000000007F; break;
  case  8 : *ptr_out = 0x00000000000000FF; break;
  case  9 : *ptr_out = 0x00000000000001FF; break;
  case 10 : *ptr_out = 0x00000000000003FF; break;
  case 11 : *ptr_out = 0x00000000000007FF; break;
  case 12 : *ptr_out = 0x0000000000000FFF; break;
  case 13 : *ptr_out = 0x0000000000001FFF; break;
  case 14 : *ptr_out = 0x0000000000003FFF; break;
  case 15 : *ptr_out = 0x0000000000007FFF; break;
  case 16 : *ptr_out = 0x000000000000FFFF; break;
  case 17 : *ptr_out = 0x000000000001FFFF; break;
  case 18 : *ptr_out = 0x000000000003FFFF; break;
  case 19 : *ptr_out = 0x000000000007FFFF; break;
  case 20 : *ptr_out = 0x00000000000FFFFF; break;
  case 21 : *ptr_out = 0x00000000001FFFFF; break;
  case 22 : *ptr_out = 0x00000000003FFFFF; break;
  case 23 : *ptr_out = 0x00000000007FFFFF; break;
  case 24 : *ptr_out = 0x0000000000FFFFFF; break;
  case 25 : *ptr_out = 0x0000000001FFFFFF; break;
  case 26 : *ptr_out = 0x0000000003FFFFFF; break;
  case 27 : *ptr_out = 0x0000000007FFFFFF; break;
  case 28 : *ptr_out = 0x000000000FFFFFFF; break;
  case 29 : *ptr_out = 0x000000001FFFFFFF; break;
  case 30 : *ptr_out = 0x000000003FFFFFFF; break;
  case 31 : *ptr_out = 0x000000007FFFFFFF; break;
  case 32 : *ptr_out = 0x00000000FFFFFFFF; break;
  case 33 : *ptr_out = 0x00000001FFFFFFFF; break;
  case 34 : *ptr_out = 0x00000003FFFFFFFF; break;
  case 35 : *ptr_out = 0x00000007FFFFFFFF; break;
  case 36 : *ptr_out = 0x0000000FFFFFFFFF; break;
  case 37 : *ptr_out = 0x0000001FFFFFFFFF; break;
  case 38 : *ptr_out = 0x0000003FFFFFFFFF; break;
  case 39 : *ptr_out = 0x0000007FFFFFFFFF; break;
  case 40 : *ptr_out = 0x000000FFFFFFFFFF; break;
  case 41 : *ptr_out = 0x000001FFFFFFFFFF; break;
  case 42 : *ptr_out = 0x000003FFFFFFFFFF; break;
  case 43 : *ptr_out = 0x000007FFFFFFFFFF; break;
  case 44 : *ptr_out = 0x00000FFFFFFFFFFF; break;
  case 45 : *ptr_out = 0x00001FFFFFFFFFFF; break;
  case 46 : *ptr_out = 0x00003FFFFFFFFFFF; break;
  case 47 : *ptr_out = 0x00007FFFFFFFFFFF; break;
  case 48 : *ptr_out = 0x0000FFFFFFFFFFFF; break;
  case 49 : *ptr_out = 0x0001FFFFFFFFFFFF; break;
  case 50 : *ptr_out = 0x0003FFFFFFFFFFFF; break;
  case 51 : *ptr_out = 0x0007FFFFFFFFFFFF; break;
  case 52 : *ptr_out = 0x000FFFFFFFFFFFFF; break;
  case 53 : *ptr_out = 0x001FFFFFFFFFFFFF; break;
  case 54 : *ptr_out = 0x003FFFFFFFFFFFFF; break;
  case 55 : *ptr_out = 0x007FFFFFFFFFFFFF; break;
  case 56 : *ptr_out = 0x00FFFFFFFFFFFFFF; break;
  case 57 : *ptr_out = 0x01FFFFFFFFFFFFFF; break;
  case 58 : *ptr_out = 0x03FFFFFFFFFFFFFF; break;
  case 59 : *ptr_out = 0x07FFFFFFFFFFFFFF; break;
  case 60 : *ptr_out = 0x0FFFFFFFFFFFFFFF; break;
  case 61 : *ptr_out = 0x1FFFFFFFFFFFFFFF; break;
  case 62 : *ptr_out = 0x3FFFFFFFFFFFFFFF; break;
  case 63 : *ptr_out = 0x7FFFFFFFFFFFFFFF; break;
  case 64 : *ptr_out = 0xFFFFFFFFFFFFFFFF; break;
  default : go_BYE(-1); break; 
  }
 BYE:
  return(status);
}


// START FUNC DECL
int
unpack(
       char *tbl,
       char *fin,
       char *str_shifts,
       char *str_dst_fldtypes,
       char *str_flds
       )
// STOP FUNC DECL
{
  int status = 0;

  char **Y = NULL; int  nY = 0;
  char **Z = NULL; int  nZ = 0;
  char **flds = NULL; int n_flds = 0;
	
  char *fin_X; size_t fin_nX = 0;

  char *Xs[MAX_PACK_FLDS]; size_t nXs[MAX_PACK_FLDS];
  int shifts[MAX_PACK_FLDS];
  unsigned long long masks[MAX_PACK_FLDS];
  FLD_REC_TYPE fin_rec, nn_fin_rec;
  FLD_REC_TYPE temp_fld_rec; int temp_fld_id;
  int fin_id, nn_fin_id; 
  FLD_REC_TYPE fld_recs[MAX_PACK_FLDS]; FLD_REC_TYPE nn_fld_recs[MAX_PACK_FLDS];
  int fld_ids[MAX_PACK_FLDS], nn_fld_ids[MAX_PACK_FLDS]; 
  FLD_TYPE dst_fldtypes[MAX_PACK_FLDS];
  char *endptr;
  TBL_REC_TYPE tbl_rec; int tbl_id; long long nR;
  char opfiles[MAX_PACK_FLDS][MAX_LEN_FILE_NAME+1]; 
  int ddir_ids[MAX_PACK_FLDS];

  // basic checks
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fin == NULL ) || ( *fin == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_shifts == NULL ) || ( *str_shifts == '\0' ) ) { go_BYE(-1); }
  if ( ( str_dst_fldtypes == NULL ) || ( *str_dst_fldtypes == '\0' ) ) { go_BYE(-1); }
  //---------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  nR = tbl_rec.nR;
  for ( int i = 0; i < MAX_PACK_FLDS; i++ ) { 
    zero_string(opfiles[i], (MAX_LEN_FILE_NAME+1));
  }
  // initializations
  for ( int i = 0; i < MAX_PACK_FLDS; i++ ) {
    Xs[i] = NULL; nXs[i] = 0;
    fld_ids[i] = -1; 
    nn_fld_ids[i] = -1;
    dst_fldtypes[i] = xunknown;
    shifts[i] = -1;
    zero_fld_rec(&(fld_recs[i])); 
    zero_fld_rec(&(nn_fld_recs[i]));
    masks[i] = 0;
  }
  // get names of fields to unpack 
  status = explode(str_flds, ':', &flds, &n_flds); cBYE(status);
  if ( n_flds <= 1 ) { go_BYE(-1); }
  if ( n_flds > MAX_PACK_FLDS ) { go_BYE(-1); }

  // get shifts for each field 
  status = explode(str_shifts, ':', &Y, &nY); cBYE(status);
  if ( nY != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    shifts[i] = strtoll(Y[i], &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( shifts[i] < 0 ) { go_BYE(-1); }
  }
  // get fldtypes for each field 
  status = explode(str_dst_fldtypes, ':', &Z, &nZ); cBYE(status);
  if ( nZ != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    status = unstr_fldtype(Z[i], &(dst_fldtypes[i])); cBYE(status);
    // allocate space for output 
    int fldsz = -1;
    status = get_fld_sz(dst_fldtypes[i], &fldsz); cBYE(status);
    size_t filesz = fldsz * nR; 
    status = mk_temp_file(opfiles[i], filesz, &(ddir_ids[i])); cBYE(status);
    status = q_mmap(ddir_ids[i], opfiles[i], &(Xs[i]), &(nXs[i]), true); 
    cBYE(status);
  }
  // get access to input 
  status = is_fld(NULL, tbl_id, fin, &fin_id, &fin_rec, &nn_fin_id,&nn_fin_rec);
  if ( ( fin_rec.fldtype != I8 ) && ( fin_rec.fldtype != I4 ) ) {
    go_BYE(-1);
  }
  status = get_data(fin_rec, &fin_X, &fin_nX, false); cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    int nbits;
    if ( i == 0 ) { 
      switch ( fin_rec.fldtype  ) {
      case I4 : nbits = 32 - shifts[i]; break;
      case I8 : nbits = 64 - shifts[i]; break;
      default : go_BYE(-1); break;
      }
    }
    else {
      nbits = shifts[i-1] - shifts[i]; 
    }
    switch ( fin_rec.fldtype  ) {
    case I4 : status = get_mask_I4(nbits, &(masks[i])); break;
    case I8 : status = get_mask_I8(nbits, &(masks[i])); break;
    default : go_BYE(-1); break;
    }
  }
  //----------------------------------------------------------
  // Here starts the core processing 
  unsigned long long *finI8 = (unsigned long long *)fin_X;
  unsigned int       *finI4 = (unsigned int       *)fin_X;
  long long block_size; int nT;
  status = partition(nR, 8192, -1, &block_size, &nT); cBYE(status);

  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    switch ( fin_rec.fldtype ) {
      case I4 : 
    for ( long long i = lb; i < ub; i++ ) {
      unsigned int inI4 = finI4[i];
      for ( int j = 0; j < n_flds; j++ ) { 
	unsigned int valI4;
	char  *tmpI1 = NULL;
	short *tmpI2 = NULL;
	int   *tmpI4 = NULL;
	valI4 = ( inI4 >> shifts[j] )  & masks[j];
	switch ( dst_fldtypes[j] ) {
	case I1 : 
	  tmpI1    = (char *)Xs[j];
	  tmpI1[i] = (char)valI4;
	  break; 
	case I2 : 
	  tmpI2    = (short *)Xs[j];
	  tmpI2[i] = (short)valI4;
	  break; 
	case I4 : 
	  tmpI4    = (int *)Xs[j];
	  tmpI4[i] = (int)valI4;
	  break; 
	default : 
	  if ( status == 0 ) { WHEREAMI; } status = -1; 
	  break;
	}
      }
    }
    break;
      case I8 : 
    for ( long long i = lb; i < ub; i++ ) {
      unsigned long long inI8 = finI8[i];
      for ( int j = 0; j < n_flds; j++ ) { 
	unsigned long long valUI8;
	char  *tmpI1 = NULL;
	short *tmpI2 = NULL;
	int   *tmpI4 = NULL;
	valUI8 = ( inI8 >> shifts[j] )  & masks[j];
	long long valI8 = (long long) valUI8;
	switch ( dst_fldtypes[j] ) {
	case I1 : 
	  if ( ( valI8 < SCHAR_MIN ) || ( valI8 > SCHAR_MAX ) ) { 
	    fprintf(stderr, "I1: valI8 out of bounds = %lld \n", valI8); 
	    status = -1; }
	  tmpI1    = (char *)Xs[j];
	  tmpI1[i] = (char)valI8;
	  break; 
	case I2 : 
	  if ( ( valI8 < SHRT_MIN ) || ( valI8 > SHRT_MAX ) ) { status=-1; 
	    fprintf(stderr, "I2: valI8 out of bounds = %lld \n", valI8); 
	  }
	  tmpI2    = (short *)Xs[j];
	  tmpI2[i] = (short)valI8;
	  break; 
	case I4 : 
	  if ( ( valI8 < INT_MIN ) || ( valI8 > INT_MAX ) ) { status=-1; 
	    fprintf(stderr, "I4: valI8 out of bounds = %lld \n", valI8); 
	  }
	  tmpI4    = (int *)Xs[j];
	  tmpI4[i] = (int)valI8;
	  break; 
	default : 
	  if ( status == 0 ) { WHEREAMI; } status = -1; 
	  break;
	}
      }
    }
    break;
      default : 
	  if ( status == 0 ) { WHEREAMI; } status = -1; 
	  break;

    }
  }
  cBYE(status);
  /* add fields */
  for ( int i = 0; i < n_flds; i++ ) { 
    status = add_fld(tbl_id, flds[i], ddir_ids[i], opfiles[i], dst_fldtypes[i], 
		     -1, &temp_fld_id, &temp_fld_rec);
    cBYE(status);
  }
 BYE:
  for ( int i = 0; i < n_flds; i++ ) {
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(Y[i]);
    free_if_non_null(Z[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(fin_X, fin_nX);
  free_if_non_null(Y);
  free_if_non_null(Z);
  free_if_non_null(flds);
  return(status);
}
