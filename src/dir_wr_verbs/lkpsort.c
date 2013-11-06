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
#include "set_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"
#include "lkpsort.h"

/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
lkpsort(
	       char *t1,
	       char *f1,
	       char *t2,
	       char *lb,
	       char *cnt,
	       char *idx_f1,
	       char *srt_f1
	       )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *srt_f1_X = NULL; size_t srt_f1_nX = 0;
  char *idx_f1_X = NULL; size_t idx_f1_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;

  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int lb_id = INT_MIN, nn_lb_id = INT_MIN;
  int cnt_id = INT_MIN, nn_cnt_id = INT_MIN;
  int srt_f1_id = INT_MIN;
  int idx_f1_id = INT_MIN;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE lb_rec, nn_lb_rec;
  FLD_REC_TYPE cnt_rec, nn_cnt_rec;
  FLD_REC_TYPE srt_f1_rec, idx_f1_rec;
  FLD_TYPE srt_fldtype, idx_fldtype; 
  long long nR1, nR2; 
  char srt_file[MAX_LEN_FILE_NAME+1];
  char idx_file[MAX_LEN_FILE_NAME+1];
  int *offsets = NULL;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( lb == NULL ) || ( *lb == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt == NULL ) || ( *cnt == '\0' ) ) { go_BYE(-1); }
  if ( ( srt_f1 == NULL ) || ( *srt_f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( idx_f1 == NULL ) || ( *idx_f1 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, srt_f1) == 0 ) { go_BYE(-1); }
  if ( strcmp(srt_f1, idx_f1) == 0 ) { go_BYE(-1); }

  zero_string(srt_file, (MAX_LEN_FILE_NAME+1));
  zero_string(idx_file, (MAX_LEN_FILE_NAME+1));
  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = t2_rec.nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, lb, &lb_id, &lb_rec, &nn_lb_id, &nn_lb_rec); 
  cBYE(status);
  chk_range(lb_id, 0, g_n_fld);
  status = get_data(lb_rec, &lb_X, &lb_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( lb_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_lb_id >= 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, cnt, &cnt_id, &cnt_rec, &nn_cnt_id, &nn_cnt_rec); 
  cBYE(status);
  chk_range(cnt_id, 0, g_n_fld);
  status = get_data(cnt_rec, &cnt_X, &cnt_nX, false); cBYE(status);
  /* Restrictions of current implementation */
  if ( cnt_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nn_cnt_id >= 0 ) { go_BYE(-1); }
  /*--------------------------------------------------------*/
  /* Make output storage */
  int fldsz = INT_MAX, srt_ddir_id = INT_MAX, idx_ddir_id = INT_MAX;
  srt_fldtype = f1_rec.fldtype;
  status = get_fld_sz(srt_fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(srt_file, (nR1 * fldsz), &srt_ddir_id); cBYE(status);
  status = q_mmap(srt_ddir_id, srt_file, &srt_f1_X, &srt_f1_nX, true); cBYE(status);

  idx_fldtype = I4;
  status = get_fld_sz(idx_fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(idx_file, (nR1 * fldsz), &idx_ddir_id); cBYE(status);
  status = q_mmap(idx_ddir_id, idx_file, &idx_f1_X, &idx_f1_nX, true); cBYE(status);
  /*--------------------------------------------------------*/
  offsets = (int *)malloc(nR2 * sizeof(int));
  return_if_malloc_failed(offsets);
  for ( int i = 0; i < nR2; i++ ) { 
    offsets[i] = ((int *)lb_X)[i];
  }
  int *f1I4 = (int *)f1_X;
  int *lbI4 = (int *)lb_X;
  int *cntI4 = (int *)cnt_X;
  int *srt_f1I4 = (int *)srt_f1_X;
  int *idx_f1I4 = (int *)idx_f1_X;
  for ( long long i = 0; i < nR1; i++ ) { 
    int I4val = f1I4[i];
    if ( ( I4val < 0 ) || ( I4val >= nR2 ) ) { go_BYE(-1); }
    int offset = offsets[I4val];
    if ( ( offset < 0 ) || ( offset >= nR1 ) ) { go_BYE(-1); }
    srt_f1I4[offset] = I4val;
    idx_f1I4[offset] = i;
    offsets[I4val] = offset + 1;
  }
  for ( int i = 0; i < nR2; i++ ) { 
    if ( ( lbI4[i] + cntI4[i] ) != offsets[i] ) {
      go_BYE(-1);
    }
  }
  /*-----------------------------------------------------------*/
  rs_munmap(srt_f1_X, srt_f1_nX);
  rs_munmap(idx_f1_X, idx_f1_nX);
  status = add_fld(t1_id, srt_f1, srt_ddir_id, srt_file, srt_fldtype, 
      -1, &srt_f1_id, &srt_f1_rec);
  cBYE(status);
  status = add_fld(t1_id, idx_f1, idx_ddir_id, idx_file, idx_fldtype, 
      -1, &idx_f1_id, &idx_f1_rec);
  cBYE(status);
  /*-----------------------------------------------------------*/
 BYE:
  free_if_non_null(offsets);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(srt_f1_X, srt_f1_nX);
  rs_munmap(idx_f1_X, idx_f1_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(cnt_X, cnt_nX);
  return(status);
}
