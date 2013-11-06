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
#include "dbauxil.h"
#include "auxil.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_tbl.h"
#include "ext_f_to_s.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
//---------------------------------------------------------------
// START FUNC DECL
int 
copy_fld(
	 char *t1,
	 char *f1,
	 char *cfld,
	 char *t2
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0; 
  char *nnopX = NULL;  size_t n_nnopX = 0; 
  char *opX = NULL;  size_t n_opX = 0; 
  char *nnX = NULL; size_t n_nnX = 0; 
  char *cfldX = NULL; size_t n_cfldX = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE t1f1_rec, nn_t1f1_rec;
  FLD_REC_TYPE t2f1_rec, nn_t2f1_rec;
  FLD_REC_TYPE cfld_rec, nn_cfld_rec;
  long long nR1 = LLONG_MIN, exp_nR2 = LLONG_MIN, nR2  = LLONG_MIN;
  long long lb = LLONG_MIN, ub = LLONG_MAX;
  int t1_id = INT_MIN, t2_id = INT_MIN; 
  int t1f1_id = INT_MIN, t2f1_id = INT_MIN;
  int cfld_id = INT_MIN, nn_cfld_id = INT_MIN; 
  int nn_t1f1_id = INT_MIN, nn_t2f1_id = INT_MIN;
  char *endptr;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  bool is_null_val_in_op = false;
#define SZ_RSLT_BUF 32
  char rslt_buf[SZ_RSLT_BUF];
  char **Y = NULL; int nY = 0;
  bool is_cfld_a_range = false;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR;

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id >= 0 ) {
    nR2 = g_tbls[t2_id].nR;
  }

  status = is_fld(NULL, t1_id, f1, &t1f1_id, &t1f1_rec, 
		  &nn_t1f1_id, &nn_t1f1_rec); cBYE(status);
  chk_range(t1f1_id, 0, g_n_fld);
  status = get_data(t1f1_rec, &X, &nX, false); cBYE(status);

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    /* See if cfld is a range or a boolean field */
    for ( char *cptr = cfld; *cptr != '\0'; cptr++ ) { 
      if ( *cptr == ':' ) {
	is_cfld_a_range = true;
	break;
      }
    }
    if ( is_cfld_a_range == true ) { 
      status = explode(cfld, ':', &Y, &nY); cBYE(status);
      if ( nY != 2 ) { go_BYE(-1); }
      lb = strtoll(Y[0], &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      ub = strtoll(Y[1], &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      exp_nR2 = ub - lb;
      if ( exp_nR2 <= 0 ) { go_BYE(-1); }
    }
    else {

      status = is_fld(NULL, t1_id, cfld, &cfld_id, &cfld_rec, 
		      &nn_cfld_id, &nn_cfld_rec); cBYE(status);
      chk_range(cfld_id, 0, g_n_fld);
      status = get_data(cfld_rec, &cfldX, &n_cfldX, 0); cBYE(status);
      if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
      if ( ( cfld_rec.fldtype != I1 ) && ( cfld_rec.fldtype != B ) ) {
	go_BYE(-1); 
      }
      if ( cfld_rec.cnt < 0 ) { 
	status = ext_f_to_s(t1, cfld, "sum", rslt_buf, SZ_RSLT_BUF);
	cBYE(status);
	status = explode(rslt_buf, ':', &Y, &nY); cBYE(status);
	if ( nY != 2 ) { go_BYE(-1); }
	exp_nR2 = strtoll(Y[0], &endptr, 10);
	if ( *endptr != '\0' ) { go_BYE(-1); }
      }
      else {
	exp_nR2 = cfld_rec.cnt;
      }
    }
  }
  else {
    exp_nR2 = nR1;
  }
  if ( t2_id >= 0 ) {
    if ( exp_nR2 != nR2 ) { 
      fprintf(stderr, "Incompatible number of rows %lld --> %lld \n",
	      exp_nR2, nR2);
      go_BYE(-1); 
    }
  }
  else  {
    nR2 = exp_nR2;
    sprintf(rslt_buf, "%lld", nR2);
    status = add_tbl(t2, rslt_buf, &t2_id, &t2_rec); cBYE(status);
  }
  // Get nn field for f1 if if it exists
  if ( nn_t1f1_id >= 0 ) { 
    status = get_data(nn_t1f1_rec, &nnX, &n_nnX, 0);
  }
  //--------------------------------------------------------
  /* Allocate space for output */
  int fldsz, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  status = get_fld_sz(t1f1_rec.fldtype, &fldsz);
  status = mk_temp_file(opfile, (nR2 * fldsz), &ddir_id);
  status = q_mmap(ddir_id, opfile, &opX, &n_opX, 1); cBYE(status);
  //--------------------------------------------------------
  /* Allocate space for output condition field if necessary */
  if ( nnX != NULL ) { 
    status = mk_temp_file(nn_opfile, (sizeof(char) * nR2), &nn_ddir_id);
    status = q_mmap(nn_ddir_id, nn_opfile, &nnopX, &n_nnopX, true); cBYE(status);
  }
  //--------------------------------------------------------
  // Get started
  if ( cfldX == NULL ) { 
    /* Optimization when (i) no condition field or (ii) range selected */
    if ( is_cfld_a_range == true ) { 
      memcpy(opX, X+(lb*fldsz), ((ub-lb) * fldsz));
      if ( nnX != NULL ) { 
	memcpy(nnopX, nnX+(lb*sizeof(char)), ((ub-lb) * sizeof(char)));
	is_null_val_in_op = true;
      }
    }
    else {
      memcpy(opX, X, (nR1 * fldsz));
      if ( nnX != NULL ) { 
	memcpy(nnopX, nnX, nR1 * sizeof(char));
	is_null_val_in_op = true;
      }
    }
  }
  else {
    char *bak_X = X, *bak_nnX = nnX, *bak_opX = opX, *bak_nnopX = nnopX;
    long long dbg_nR2 = 0;
    char      *opI1 = (char      *)opX; char      *ipI1 = (char *)X;
    short     *opI2 = (short     *)opX; short     *ipI2 = (short *)X;
    int       *opI4 = (int       *)opX; int       *ipI4 = (int *)X;
    long long *opI8 = (long long *)opX; long long *ipI8 = (long long *)X;
    if ( nnX == NULL ) { 
      switch ( t1f1_rec.fldtype ) { 
	case I1 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI1[dbg_nR2++] = ipI1[i];
	    }
	  }
	  break;
	case I2 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI2[dbg_nR2++] = ipI2[i];
	    }
	  }
	  break;
	case I4 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI4[dbg_nR2++] = ipI4[i];
	    }
	  }
	  break;
	case I8 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI8[dbg_nR2++] = ipI8[i];
	    }
	  }
	  break;
	case F4 :
	case F8 : 
	  fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
	  break;
	default : go_BYE(-1); break;
      }

      /* OLD 
      for ( long long i = 0; i < nR1; i++ ) {
	if ( cfldX[i] == TRUE ) { 
	  memcpy(opX, X, fldsz);
	  opX += fldsz;
	  dbg_nR2++;
	}
	X += fldsz;
      }
      */
    }
    else {
      for ( long long i = 0; i < nR1; i++ ) {
	if ( cfldX[i] == TRUE ) { 
	  memcpy(opX, X, fldsz);
	  opX += fldsz;
	  *nnopX = *nnX;
	  if ( *nnopX == FALSE ) { is_null_val_in_op = true; }
	  nnopX++;
	  nnX++;
	  dbg_nR2++;
	}
	X += fldsz;
      }
    }
    if ( dbg_nR2 != nR2 ) { go_BYE(-1); } 
    X     = bak_X;
    nnX   = bak_nnX;
    opX   = bak_opX;
    nnopX = bak_nnopX;
  }

  // Add field to meta data store 
  status = add_fld(t2_id, f1, ddir_id, opfile, t1f1_rec.fldtype, -1, 
      &t2f1_id, &t2f1_rec);
  cBYE(status);
  /*- Some other meta data to take care of --------------------*/
  /* Sort status stays the same */
  g_flds[t2f1_id].sort_type = g_flds[t1f1_id].sort_type;
  g_flds[t2f1_id].dict_tbl_id = g_flds[t1f1_id].dict_tbl_id;
  /*-----------------------------------------------------------*/
  if ( ( nnX != NULL ) && ( is_null_val_in_op ) ) {
    status = add_aux_fld(NULL, t2_id ,NULL, t2f1_id, nn_ddir_id, nn_opfile, 
			 "nn", &nn_t2f1_id, &nn_t2f1_rec);
    cBYE(status);
  }
  else {
    if ( nn_opfile != NULL ) { unlink(nn_opfile); }
  }
 BYE:
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) { 
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  rs_munmap(X, nX);
  rs_munmap(nnX, n_nnX);
  rs_munmap(opX, n_opX);
  rs_munmap(nnopX, n_nnopX);
  rs_munmap(cfldX, n_cfldX);
  return(status);
}
