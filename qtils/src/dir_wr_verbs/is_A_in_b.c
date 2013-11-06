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
#include "aux_meta.h"
#include "auxil.h"
#include "dbauxil.h"
#include "bin_search_I4.h"
#include "bin_search_I8.h"
#include "get_nR.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_nn_fld.h"
#include "meta_globals.h"
#include "ext_f_to_s.h"
#include "is_A_in_b.h"
#include "find_largest_leq_I4.h"
#include "find_smallest_geq_I4.h"

/* This is used when f1 and f2 are non-null, sorted ascending */
/* Usually used when T1 is large and T2 is small */

//---------------------------------------------------------------
// START FUNC DECL
int 
is_A_in_b(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *cfX = NULL; size_t cf_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  FLD_REC_TYPE f1_rec, f2_rec, cfld_rec;
  FLD_REC_TYPE nn_f1_rec, nn_f2_rec;
  TBL_REC_TYPE t1_rec, t2_rec;
  long long nR1, nR2;
  char opfile[MAX_LEN_FILE_NAME+1];
  int t1_id, t2_id; 
  int f1_id, nn_f1_id; 
  int f2_id, nn_f2_id; 
  int cfld_id = INT_MIN;
  int ddir_id;
#define BUFSZ 16
  char buf[BUFSZ];
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR; if ( nR1 <= 0 ) { go_BYE(-1); }
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  if ( f1_id < 0 ) { 
    fprintf(stderr, "Field [%s] in Table [%s] missing\n", f1, t1);
    go_BYE(-1); 
  }
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  status = ext_f_to_s(t1, f1, "is_sorted", buf, BUFSZ); cBYE(status);
  if ( strcmp(buf, "ascending") != 0 ) { go_BYE(-1); }
  //------------------------------------------------

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  status = ext_f_to_s(t2, f2, "is_sorted", buf, BUFSZ); cBYE(status);
  if ( strcmp(buf, "ascending") != 0 ) { go_BYE(-1); }

  //--------------------------------------------------------
  if (f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  // There can be null values in f1 but not in f2
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  /*------------------------------------------------------------*/
  /* Make space for output condition field */
  status = mk_temp_file(opfile, (nR1 *sizeof(char)), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &cfX, &cf_nX, 1); cBYE(status);
  /*------------------------------------------------------------------*/
  long long min_block_size = 8192, block_size; int nT;
  status = partition(nR1, min_block_size, -1, &block_size, &nT); cBYE(status);
  for ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR1; }
    // These are kept local to avoid sharing issues
    int       *f2_I4 = (int       *)f2_X;
    char *l_cfX = cfX; // make a local copy
    int       *f1_I4 = (int *)f1_X; // make a local copy
    int       min_val_this_block_I4;
    long long min_f2_idx;
    switch ( f1_rec.fldtype ) { 
      case I4 : 
	min_val_this_block_I4 = f1_I4[lb];
	status = find_largest_leq_I4 (f2_I4, nR2, min_val_this_block_I4, 
	      &min_f2_idx);
	break;
      default : 
        if ( status == 0 ) { WHEREAMI; } status = -1;
	break;
    }
    // TODO: Do optimization if all values in this block are null 
    int       *l_f2_I4 = f2_I4 + min_f2_idx;
    int *l_max_f2_I4   = f2_I4 + nR2;
    //-----------------------------------
    for  ( long long i1 = lb; i1 < ub; i1++ ) {
      /* If f1 is null, then cfld must also be null */
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
	l_cfX[i1] = FALSE;
	continue;
      }
      /*----------------------------------------------------------------*/
      int       f1_val_I4 = f1_I4[i1];
      int found = FALSE;
      switch ( f1_rec.fldtype ) { 
      case I4 : 
      for ( ; l_f2_I4 < l_max_f2_I4; ) { 
	int f2_val_I4 = *l_f2_I4;
	if ( f2_val_I4 < f1_val_I4 ) {
	  l_f2_I4++;
	  continue;
	}
	else if ( f2_val_I4 == f1_val_I4 ) {
	  found = TRUE;
	  break;
	}
	else {
	  found = FALSE;
	  break;
	}
      }
      break;
      default  :
      if ( status == 0 ) { WHEREAMI; } status = -1;
      break;
      }
      l_cfX[i1] = found;
    }
  }
  cBYE(status);

  // Add output field to meta data 
  status = add_fld(t1_id, cfld, ddir_id, opfile, I1, -1, &cfld_id, &cfld_rec);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(cfX, cf_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  return(status);
}
