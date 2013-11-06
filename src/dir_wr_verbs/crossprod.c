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


//---------------------------------------------------------------
// START FUNC DECL
int 
crossprod(
	  char *t1,
	  char *f1,
	  char *f2,
	  char *t2,
	  char *t2f1,
	  char *t2f2,
	  char *taux,
	  char *grp_cnt
	  )
// STOP FUNC DECL
{
  int status = 0;
  long long nR1, nR2, nRaux = 0;
  int t1_id, t2_id, taux_id;
  TBL_REC_TYPE t1_rec, t2_rec, taux_rec;

  FLD_REC_TYPE t1f1_rec, nn_t1f1_rec; int t1f1_id, nn_t1f1_id;
  FLD_REC_TYPE t1f2_rec, nn_t1f2_rec; int t1f2_id, nn_t1f2_id;

  FLD_REC_TYPE grp_cnt_rec, nn_grp_cnt_rec; int grp_cnt_id, nn_grp_cnt_id;

  FLD_REC_TYPE t2f1_rec; int t2f1_id;
  FLD_REC_TYPE t2f2_rec; int t2f2_id;

  char *t1f1_X = NULL;   size_t t1f1_nX = 0;
  char *t1f2_X = NULL;   size_t t1f2_nX = 0;
  char *grp_cnt_X = NULL;   size_t grp_cnt_nX = 0;
  char *t2f1_X = NULL; size_t t2f1_nX = 0;
  char *t2f2_X = NULL; size_t t2f2_nX = 0;

  int *cntI4 = NULL;
  char t2f1_opfile[MAX_LEN_FILE_NAME+1], t2f2_opfile[MAX_LEN_FILE_NAME+1];
  int t2f1_ddir_id, t2f2_ddir_id; size_t filesz; int fldsz;

  bool is_aux;

  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); } 
  if ( ( taux == NULL ) || ( *taux == '\0' ) ) { 
    if ( ( grp_cnt != NULL ) && ( *grp_cnt != '\0' ) ) { go_BYE(-1); }
    is_aux = false;
  }
  else {
    if ( strcmp(taux, t2) == 0 ) { go_BYE(-1); } 
    if ( strcmp(taux, t1) == 0 ) { go_BYE(-1); } 
    if ( ( grp_cnt == NULL ) || ( *grp_cnt == '\0' ) ) { go_BYE(-1); }
    is_aux = true;
  }
  zero_string(t2f1_opfile, MAX_LEN_FILE_NAME+1);
  zero_string(t2f2_opfile, MAX_LEN_FILE_NAME+1);

  if ( ( t2f1 == NULL ) || ( *t2f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2f2 == NULL ) || ( *t2f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t2f1, t2f2) == 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR; 

  status = is_fld(NULL, t1_id, f1, &t1f1_id, &t1f1_rec, &nn_t1f1_id, &nn_t1f1_rec);
  chk_range(t1f1_id, 0, g_n_fld);
  status = get_data(t1f1_rec, &t1f1_X, &t1f1_nX, false); cBYE(status);
  if ( nn_t1f1_id >= 0 ) { go_BYE(-1); }
  if ( t1f1_rec.fldtype != I4 ) { go_BYE(-1); }
  //--------------------------------------------------------
  if ( strcmp(f2, f1) == 0 ) {
    t1f2_X = t1f1_X;
  }
  else {
    status = is_fld(NULL, t1_id, f2, &t1f2_id, &t1f2_rec, &nn_t1f2_id, &nn_t1f2_rec);
    chk_range(t1f2_id, 0, g_n_fld);
    status = get_data(t1f2_rec, &t1f2_X, &t1f2_nX, false); cBYE(status);
    if ( nn_t1f2_id >= 0 ) { go_BYE(-1); }
    if ( t1f2_rec.fldtype != I4 ) { go_BYE(-1); }
  
    if ( t1f2_rec.fldtype != t1f1_rec.fldtype ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  status = del_tbl(t2, -1); 
  //--------------------------------------------------------
  if ( is_aux == true ) { 
    status = is_tbl(taux, &taux_id, &taux_rec); cBYE(status);
    chk_range(taux_id, 0, g_n_tbl);
    nRaux = taux_rec.nR; 

    status = is_fld(NULL, taux_id, grp_cnt, &grp_cnt_id, &grp_cnt_rec, &nn_grp_cnt_id, &nn_grp_cnt_rec);
    chk_range(grp_cnt_id, 0, g_n_fld);
    status = get_data(grp_cnt_rec, &grp_cnt_X, &grp_cnt_nX, false); cBYE(status);
    if ( nn_grp_cnt_id >= 0 ) { go_BYE(-1); }
    if ( grp_cnt_rec.fldtype != I4 ) { go_BYE(-1); }

    cntI4 = (int *)grp_cnt_X;
    long long sum = 0;
    nR2 = 0; 
    for ( int i = 0;  i < nRaux; i++ ) {
      if ( ( cntI4[i] <= 0 ) ||  ( cntI4[i] >= nR1 ) ) { go_BYE(-1); }
      sum += cntI4[i];
      nR2 += (cntI4[i] * cntI4[i]);
    }
    if ( sum != nR1 ) { go_BYE(-1); }
    if ( nR2 > (nR1 * nR1) ) { go_BYE(-1); }
  }
  else {
    nR2 = nR1 * nR1;
  }
  // allocate space for output
  status = get_fld_sz(t1f1_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR2;

  status = mk_temp_file(t2f1_opfile, filesz, &t2f1_ddir_id); cBYE(status);
  status = q_mmap(t2f1_ddir_id, t2f1_opfile, &t2f1_X, &t2f1_nX, true); cBYE(status);

  status = mk_temp_file(t2f2_opfile, filesz, &t2f2_ddir_id); cBYE(status);
  status = q_mmap(t2f2_ddir_id, t2f2_opfile, &t2f2_X, &t2f2_nX, true); cBYE(status);

  char buffer[32];
  sprintf(buffer, "%lld", nR2);
  status = add_tbl(t2, buffer, &t2_id, &t2_rec); cBYE(status);

  /* create the new fields */
  int t1f1I4, t1f2I4;
  int *t2f1I4ptr = (int *)t2f1_X;
  int *t2f2I4ptr = (int *)t2f2_X;
  int *t1f1I4ptr = (int *)t1f1_X;
  int *t1f2I4ptr = (int *)t1f2_X;
  long long chk_nR2 = 0;

  if ( is_aux == true ) { 
  int lbidx = 0, ubidx;
  for ( int grp = 0; grp < nRaux; grp++ ) { 
    ubidx = lbidx + cntI4[grp];
    for ( int j = lbidx; j < ubidx; j++ ) {
      t1f1I4 = t1f1I4ptr[j];
      for ( int k = lbidx; k < ubidx; k++ ) { // TODO P0: Document the +1
        t1f2I4 = t1f2I4ptr[k];
	t2f1I4ptr[chk_nR2] = t1f1I4;
	t2f2I4ptr[chk_nR2] = t1f2I4;
	chk_nR2++;
      }
    }
    lbidx = ubidx;
  }
  }
  else {
    for ( int j = 0; j < nR1; j++ ) { 
      t1f1I4 = t1f1I4ptr[j];
      for ( int k = 0; k < nR1; k++ ) { 
        t1f2I4 = t1f2I4ptr[k];
	t2f1I4ptr[chk_nR2] = t1f1I4;
	t2f2I4ptr[chk_nR2] = t1f2I4;
	chk_nR2++;
      }
    }
  }
  if ( nR2 != chk_nR2 ) { go_BYE(-1); }
  /* add fields */
  status = add_fld(t2_id, t2f1, t2f1_ddir_id, t2f1_opfile, t1f1_rec.fldtype, 
      -1, &t2f1_id, &t2f1_rec);
  cBYE(status);
  status = add_fld(t2_id, t2f2, t2f2_ddir_id, t2f2_opfile, t1f1_rec.fldtype, 
      -1, &t2f2_id, &t2f2_rec);
  cBYE(status);

 BYE:
  rs_munmap(t1f1_X, t1f1_nX);
  if ( ( f1 != NULL ) && ( f2 != NULL ) ) {
    if ( strcmp(f2, f1) != 0 ) {
      rs_munmap(t1f2_X, t1f2_nX);
    }
  }
  rs_munmap(t2f1_X, t2f1_nX);
  rs_munmap(t2f2_X, t2f2_nX);
  return(status);
}
