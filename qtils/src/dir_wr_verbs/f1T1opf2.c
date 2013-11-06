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
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "get_nR.h"
#include "f1T1opf2.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "which_bin_I4.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f1T1opf2(
	 char *tbl,
	 char *f1,
	 char *T1,
	 char *op,
	 char *f2
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  char *lb_X; size_t lb_nX = 0;
  char *ub_X; size_t ub_nX = 0;
  long long nR = LLONG_MAX, nR2 = LLONG_MAX;
  TBL_REC_TYPE tbl_rec; int tbl_id; 
  TBL_REC_TYPE T1_rec; int T1_id; 
  FLD_REC_TYPE f1_rec, nn_f1_rec; int f1_id, nn_f1_id;
  FLD_REC_TYPE f2_rec, nn_f2_rec; int f2_id, nn_f2_id;
  FLD_REC_TYPE lb_rec, nn_lb_rec; int lb_id, nn_lb_id;
  FLD_REC_TYPE ub_rec, nn_ub_rec; int ub_id, nn_ub_id;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( T1 == NULL ) || ( *T1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  zero_string(nn_opfile, MAX_LEN_FILE_NAME+1);
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = tbl_rec.nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Get nn field for f1 if if it exists
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); 
    cBYE(status);
  } 
  status = is_tbl(T1, &T1_id, &T1_rec); cBYE(status);
  chk_range(T1_id, 0, g_n_tbl);
  nR2 = T1_rec.nR;
  //-- Allocate space for output 
  int fldsz = INT_MAX, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);

  status = mk_temp_file(opfile, (nR * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, true); cBYE(status);

  status = mk_temp_file(nn_opfile, (nR * sizeof(char)), &nn_ddir_id); cBYE(status);
  status = q_mmap(nn_ddir_id, nn_opfile, &nn_f2_X, &nn_f2_nX, true); cBYE(status);

  if ( strcmp(op, "bin") == 0 ) {
    status = is_fld(NULL, T1_id, "lb", &lb_id, &lb_rec, &nn_lb_id, &nn_lb_rec);
    chk_range(lb_id, 0, g_n_fld);
    if ( nn_lb_id >= 0 ) { go_BYE(-1); }
    status = get_data(lb_rec, &lb_X, &lb_nX, false); cBYE(status);

    status = is_fld(NULL, T1_id, "ub", &ub_id, &ub_rec, &nn_ub_id, &nn_ub_rec);
    chk_range(ub_id, 0, g_n_fld);
    if ( nn_ub_id >= 0 ) { go_BYE(-1); }
    status = get_data(ub_rec, &ub_X, &ub_nX, false); cBYE(status);

    if ( lb_rec.fldtype != I4 ) { go_BYE(-1); }
    int *lbI4 = (int *)lb_X;
    if ( ub_rec.fldtype != I4 ) { go_BYE(-1); }
    int *ubI4 = (int *)ub_X;
    if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
    int *f1I4 = (int *)f1_X;

    for ( long long i = 0; i < nR; i++ ) { 
      bool exists = false;
      register int valI4 = f1I4[i];
      int t2idx;
      register int *f2I4 = (int *)f2_X;
      register char *f2nn = nn_f2_X;
      // TODO P2: Need to speed up this loop. Can be smarter than this
      which_bin_I4(valI4, lbI4, ubI4, nR2, &t2idx, &exists);
      /* Following replaced by which_bin function
      t2idx = 0;
      for ( t2idx = 0; t2idx < nR2; t2idx++ ) { 
	if ( ( valI4 >= lbI4[t2idx] ) &&  ( valI4 <= ubI4[t2idx] ) ) {
	  found = true;
	  break;
	}
      }
      */
      if ( exists ) { 
	f2nn[i] = TRUE;
	f2I4[i] = t2idx;
      }
      else {
	f2nn[i] = FALSE;
	f2I4[i] = 0;
      }
    }
  }
  else if ( strcmp(op, "regroup") == 0 ) {
  }
  else {
    go_BYE(-1);
  }

  //--------------------------------------------------------
  status = add_fld(tbl_id, f2, ddir_id, opfile, I4, -1, &f2_id, &f2_rec);
  cBYE(status);
  if ( *nn_opfile != '\0' ) { /* there is a nn file */
    status = add_aux_fld(NULL, tbl_id, NULL, f2_id, nn_ddir_id, nn_opfile, 
	"nn", &nn_f2_id, &nn_f2_rec);
    cBYE(status);
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  return(status);
}
