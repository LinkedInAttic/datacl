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
#include <stdlib.h>
#include "qtypes.h"
#include "mmap.h"
#include "mix.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "subsample.h"

extern void srand48(long int seedval);
extern long int mrand48(void);



//---------------------------------------------------------------
// START FUNC DECL
int 
subsample(
	  char *t1,
	  char *f1,
	  char *str_nR2,
	  char *t2,
	  char *f2
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE f1_rec, f2_rec;
  FLD_REC_TYPE nn_f1_rec;
  long long nR1, nR2; 
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN;
  int nn_f1_id = INT_MIN;
  char opfile[MAX_LEN_FILE_NAME+1];
  int fldsz = INT_MAX, ddir_id = INT_MAX;
   long long chk_nR2 = 0;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  char *endptr;
  nR2 = strtoll(str_nR2, &endptr, 10);
  if ( nR2 <= 0 ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Not implemented for following cases 
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
  //---------------------------------------------
  int *max_I4f1 = (int *)f1_X; max_I4f1 += nR1;
  if ( nR1 <= nR2 ) { go_BYE(-1); }

  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(opfile, (nR2 * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, 1);  cBYE(status);

  long long block_size; int nT;
  status = partition(nR2, 1024, -1, &block_size, &nT); cBYE(status);
  long long block_size_1 = (nR1 / nT);
  int ratio = nR1 / nR2; if ( ratio == 1 ) { ratio++; }

  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb1 = tid * block_size_1; /* input lower bound */
    long long ub1 = lb1 + block_size_1; /* input upper bound */
    if ( ub1 > nR1 ) { ub1 = nR1; } /* corner case */
    if ( ub1 <= lb1 ) { WHEREAMI; status = -1; continue; }

    long long lb2 = 0 + (tid * block_size);
    long long ub2 = lb2 + block_size;
    if ( tid == (nT-1) ) { ub2 = nR2; }
    long long range2 = ub2 - lb2;

    srand48(2059879141*(tid+1)); 
    unsigned int rnum = (unsigned int)mrand48();

    int       *I4f1 = NULL, *I4f2 = NULL, *max_I4f1 = NULL;
    long long *I8f1 = NULL, *I8f2 = NULL, *max_I8f1 = NULL;
    max_I4f1 = I4f1 + ub1;
    max_I8f1 = I8f1 + ub1;
    switch ( f1_rec.fldtype ) { 
      case I4 : 
#include "incl_subsample_I4.c"
	break; 
      case I8 : 
#include "incl_subsample_I8.c"
	break; 
      default : 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	break;
    }
  }
  cBYE(status);
  //--- Add to t2
  status  = del_tbl(t2, -1); cBYE(status);
  char strbuf[32];
  sprintf(strbuf, "%lld", nR2);
  status = add_tbl(t2, strbuf, &t2_id, &t2_rec); cBYE(status);
  status = add_fld(t2_id, f2, ddir_id, opfile, f1_rec.fldtype, -1, &f2_id, &f2_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
