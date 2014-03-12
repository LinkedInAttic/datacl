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
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_tbl.h"
#include "nn_f_to_s_sum_I1.h"
#include "nn_f_to_s_sum_I2.h"
#include "nn_f_to_s_sum_I4.h"
#include "nn_f_to_s_sum_I8.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f1f2_to_s(
	  const char *tbl,
	  const char *f1,
	  const char *f2,
	  const char *op,
	  char *rslt_buf,
	  int sz_rslt_buf
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
#define MAX_LEN 32
  long long nR;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  FLD_REC_TYPE f1_rec; int f1_id; 
  FLD_REC_TYPE f2_rec; int f2_id; 
  FLD_REC_TYPE nn_f1_rec; int nn_f1_id; 
  FLD_REC_TYPE nn_f2_rec; int nn_f2_id; 
#define MAX_NUM_THREADS 16
  long long rslt[MAX_NUM_THREADS], cnt[MAX_NUM_THREADS];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1  == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2  == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(tbl, -1, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  //-------------------------------------------------------------
  status = is_fld(tbl, -1, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  cBYE(status);
  if ( f2_id < 0 ) { go_BYE(-1); }
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  //-------------------------------------------------------------
  long long block_size; int nT;
  status = partition(nR, 1, MAX_NUM_THREADS, &block_size, &nT); cBYE(status);
  if ( strcmp(op, "sum") == 0 ) { 
    for ( int tid = 0; tid < MAX_NUM_THREADS; tid++ ) { rslt[tid] = 0; }
    for ( int tid = 0; tid < MAX_NUM_THREADS; tid++ ) {  cnt[tid] = 0; }
    if ( f1_rec.fldtype != I1 ) { go_BYE(-1); }
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( nn_f2_id >= 0 ) { go_BYE(-1); }

    for ( int tid = 0; tid < nT; tid++ ) {
      if ( status < 0 ) { continue; }
      long long lb = tid * block_size;
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = nR; }
      long long tnR = ub - lb;
      char      *f2I1 = (char *)     f2_X; f2I1 += lb;
      short     *f2I2 = (short *)    f2_X; f2I2 += lb;
      int       *f2I4 = (int *)      f2_X; f2I4 += lb;
      long long *f2I8 = (long long *)f2_X; f2I8 += lb;
      long long trsltI8 = 0, tcntI8 = 0;
      switch ( f2_rec.fldtype ) {
      case I1 : 
	nn_f_to_s_sum_I1(f2I1, f1_X+lb, tnR, &tcntI8, &trsltI8);
	break;
      case I2 : 
	nn_f_to_s_sum_I2(f2I2, f1_X+lb, tnR, &tcntI8, &trsltI8);
	break;
      case I4 : 
	nn_f_to_s_sum_I4(f2I4, f1_X+lb, tnR, &tcntI8, &trsltI8);
	break;
      case I8 : 
	nn_f_to_s_sum_I8(f2I8, f1_X+lb, tnR, &tcntI8, &trsltI8);
	break;
      default : 
	status = -1; 
	break;
      }
      rslt[tid] = trsltI8;
      cnt[tid]  = tcntI8;
    }
    cBYE(status);
    long long final_rslt = 0, final_cnt = 0;
    for ( int tid = 0; tid < nT; tid++ ) { 
      final_rslt += rslt[tid];
      final_cnt  += cnt[tid];
    }
    sprintf(rslt_buf, "%lld:%lld:%lld\n", final_rslt, final_cnt, nR);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
