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
#include "dbauxil.h"
#include "aux_meta.h"
#include "set_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "meta_globals.h"
#include "rng_sort.h"

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#include "qsort_asc_I8.h"
#include "qsort_dsc_I8.h"


#define ASCENDING  1
#define DESCENDING 2
/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
rng_sort(
	 char *t1,
	 char *f1,
	 char *mode, /* A for ascending, D for descending */
	 char *taux,
	 char *lb,
	 char *cnt
	 )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X  = NULL; size_t f1_nX  = 0;
  char *lb_X  = NULL; size_t lb_nX  = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;

  char *nn_lb_X  = NULL; size_t nn_lb_nX  = 0;
  char *nn_cnt_X = NULL; size_t nn_cnt_nX = 0;

  int t1_id = INT_MIN, taux_id = INT_MIN;
  int f1_id = INT_MIN,  nn_f1_id = INT_MIN;
  int lb_id = INT_MIN,  nn_lb_id = INT_MIN;
  int cnt_id = INT_MIN, nn_cnt_id = INT_MIN;
  TBL_REC_TYPE t1_rec,  taux_rec;
  FLD_REC_TYPE f1_rec,  nn_f1_rec;
  FLD_REC_TYPE lb_rec,  nn_lb_rec;
  FLD_REC_TYPE cnt_rec, nn_cnt_rec;
  long long nR1, nRaux;  int imode;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( taux == NULL ) || ( *taux == '\0' ) ) { go_BYE(-1); }
  if ( ( mode == NULL ) || ( *mode == '\0' ) ) { go_BYE(-1); }
  if ( ( lb == NULL ) || ( *lb == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt == NULL ) || ( *cnt == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, taux) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb, cnt) == 0 ) { go_BYE(-1); }
  if ( strcmp(mode,"A") == 0 ) { 
    imode = ASCENDING; 
  }
  else if ( strcmp(mode,"D") == 0 ) { 
    imode = DESCENDING; 
  }
  else {
    go_BYE(-1);
  }

  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, true); cBYE(status);
  /* Restrictions of current implementation */
  if ( ( f1_rec.fldtype != I4 ) && ( f1_rec.fldtype != I8 ) ) { 
    go_BYE(-1); 
  }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  status = is_tbl(taux, &taux_id, &taux_rec); cBYE(status);
  chk_range(taux_id, 0, g_n_tbl);
  nRaux = taux_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, taux_id, lb, &lb_id, &lb_rec, &nn_lb_id, &nn_lb_rec); 
  cBYE(status);
  chk_range(lb_id, 0, g_n_fld);
  status = get_data(lb_rec, &lb_X, &lb_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( lb_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_lb_id >= 0 ) { 
    status = get_data(nn_lb_rec, &nn_lb_X, &nn_lb_nX, false); cBYE(status);
  }
  /*--------------------------------------------------------*/
  status = is_fld(NULL, taux_id, cnt, &cnt_id, &cnt_rec, &nn_cnt_id, &nn_cnt_rec); 
  cBYE(status);
  chk_range(cnt_id, 0, g_n_fld);
  status = get_data(cnt_rec, &cnt_X, &cnt_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( cnt_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_cnt_id >= 0 ) { 
    status = get_data(nn_cnt_rec, &nn_cnt_X, &nn_cnt_nX, false); cBYE(status);
  }
  /*--------------------------------------------------------*/
  /* Make output storage */
  int *lbI4  = (int *)lb_X;
  int *cntI4 = (int *)cnt_X;
  switch ( f1_rec.fldtype ) { 
    case I4 : 
  for ( int i = 0; i < nRaux; i++ ) {
    int *f1I4 = (int *)f1_X; 
    if ( ( nn_lb_X  != NULL ) && ( nn_lb_X[i]  == 0 ) ) { continue; }
    if ( ( nn_cnt_X != NULL ) && ( nn_cnt_X[i] == 0 ) ) { continue; }
    f1I4 += lbI4[i];
    if ( cntI4[i] == 1 ) { continue; } // nothing to sort
    if ( cntI4[i] == 2 ) { // simple swap will work 
      if ( f1I4[0] > f1I4[1] ) {
	int swapI4 = f1I4[0];
	f1I4[0] = f1I4[1];
	f1I4[1] = swapI4;
      }
      continue; 
    }
    switch ( imode ) { 
    case ASCENDING : 
      qsort_asc_I4(f1I4, cntI4[i], sizeof(int), NULL);
      break;
    case DESCENDING : 
      qsort_dsc_I4(f1I4, cntI4[i], sizeof(int), NULL);
      break;
    default : 
      break;
    }
  }
  break;
    case I8 : 
  for ( int i = 0; i < nRaux; i++ ) {
    long long *f1I8 = (long long *)f1_X; 
    f1I8 += lbI4[i];
    if ( cntI4[i] == 0 ) { continue; } // nothing to sort
    if ( cntI4[i] == 1 ) { continue; } // nothing to sort
    if ( cntI4[i] == 2 ) { // simple swap will work 
      if ( f1I8[0] > f1I8[1] ) {
	int swapI4 = f1I8[0];
	f1I8[0] = f1I8[1];
	f1I8[1] = swapI4;
      }
      continue; 
    }
    switch ( imode ) { 
    case ASCENDING : 
      qsort_asc_I8(f1I8, cntI4[i], sizeof(int), NULL);
      break;
    case DESCENDING : 
      qsort_dsc_I8(f1I8, cntI4[i], sizeof(int), NULL);
      break;
    default : 
      break;
    }
  }
  break;
    default :
  go_BYE(-1);
  break;
  }
  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(f1_X,  f1_nX);
  rs_munmap(lb_X,  lb_nX);
  rs_munmap(cnt_X, cnt_nX);
  rs_munmap(nn_lb_X,  nn_lb_nX);
  rs_munmap(nn_cnt_X, nn_cnt_nX);
  return(status);
}
