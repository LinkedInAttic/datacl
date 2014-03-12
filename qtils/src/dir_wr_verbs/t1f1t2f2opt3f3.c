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
#include "t1f1t2f2opt3f3.h"

#include "assign_I4.h"
#include "extract_I4.h"
#include "extract_I8.h"
#include "extract_S.h"

#include "pos_copy_I4_I4.h"
#include "pos_copy_I4_I8.h"
#include "pos_copy_I8_I4.h"
#include "pos_copy_I8_I8.h"

#include "pos_count_bitvec_I4_I4.h"
#include "pos_count_bitvec_I4_I8.h"
#include "pos_count_bitvec_I8_I4.h"
#include "pos_count_bitvec_I8_I8.h"

#include "pos_nn_copy_I1_I4.h"
#include "pos_nn_copy_I1_I8.h"
#include "pos_nn_copy_I4_I4.h"
#include "pos_nn_copy_I4_I8.h"
#include "pos_nn_copy_I8_I4.h"
#include "pos_nn_copy_I8_I8.h"

#include "pos_sel_copy_I4_I4.h"
#include "pos_sel_copy_I4_I8.h"
#include "pos_sel_copy_I8_I4.h"
#include "pos_sel_copy_I8_I8.h"

#include "pos_sel_aux_copy_I4_I4.h"
#include "pos_sel_aux_copy_I4_I8.h"
#include "pos_sel_aux_copy_I8_I4.h"
#include "pos_sel_aux_copy_I8_I8.h"

#include "pos_sel_aux_nn_copy_I4_I4.h"
#include "pos_sel_aux_nn_copy_I4_I8.h"
#include "pos_sel_aux_nn_copy_I8_I4.h"
#include "pos_sel_aux_nn_copy_I8_I8.h"

#include "pos_count_I4_I4.h"
#include "pos_count_I4_I8.h"
#include "pos_count_I8_I4.h"
#include "pos_count_I8_I8.h"

#include "pos_mmad_I4_I4.h"
#include "pos_mmad_I4_I8.h"
#include "pos_mmad_I8_I4.h"
#include "pos_mmad_I8_I8.h"

#include "pos_mmad_I4_I4_nn.h"
#include "pos_mmad_I4_I8_nn.h"
#include "pos_mmad_I8_I4_nn.h"
#include "pos_mmad_I8_I8_nn.h"

#include "union_I4.h"
#include "union_I8.h"

extern int g_num_cores;
extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
/*---------------------------------------------------------------*/
/* START FUNC DECL */
int 
t1f1t2f2opt3f3(
	       char *t1,
	       char *f1,
	       char *t2,
	       char *f2,
	       char *op,
	       char *options,
	       char *t3,
	       char *f3,
	       char *rslt_buf,
	       int sz_rslt_buf
	       )
/* STOP FUNC DECL */
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;
  char *sel_fld_X = NULL; size_t sel_fld_nX = 0;
  char *nn_sel_fld_X = NULL; size_t nn_sel_fld_nX = 0;
  int t1_id = INT_MIN, t2_id = INT_MIN, t3_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN, f3_id = INT_MIN;
  int nn_f1_id = INT_MIN, nn_f2_id = INT_MIN;
  int sel_fld_id = INT_MIN, nn_sel_fld_id = INT_MIN;
  TBL_REC_TYPE t1_rec, t2_rec, t3_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec, nn_f2_rec;
  FLD_REC_TYPE sel_fld_rec, nn_sel_fld_rec;
  FLD_REC_TYPE f3_rec;
  FLD_TYPE f3_type = unknown;
  long long nR1 = INT_MIN, nR2 = INT_MIN; 
  long long nR3 = INT_MIN, nR3_allocated = INT_MIN;
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];
  int f3_fldsz = INT_MAX, ddir_id = INT_MAX;
  size_t filesz;
  /*----------------------------------------------------------------*/
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }

  if ( strcmp(op, "pos_mmad") == 0 ) { 
    /* no t3 specified  */
  }
  else {
    if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(t1, t3) == 0 ) { go_BYE(-1); }
    if ( ( t3 == NULL ) || ( *t3 == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(t2, t3) == 0 ) { go_BYE(-1); }
  }

  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(nn_opfile, (MAX_LEN_FILE_NAME+1));
  /*--------------------------------------------------------*/
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { 
    status = get_data(nn_f1_rec, &nn_f1_X, &nn_f1_nX, false); cBYE(status);
  }
  /*--------------------------------------------------------*/
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbls[t2_id].nR;
  /*--------------------------------------------------------*/
  status = is_fld(NULL, t2_id, f2, &f2_id, &f2_rec, &nn_f2_id, &nn_f2_rec); 
  cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  status = get_data(f2_rec, &f2_X, &f2_nX, false); cBYE(status);
  if ( nn_f2_id >= 0 ) { 
    status = get_data(nn_f2_rec, &nn_f2_X, &nn_f2_nX, false); cBYE(status);
  }
  /*--------------------------------------------------------*/
  /* Make space for output */
  /* Note thgat nR1 + nR2 is upper bound. May need to trim this later*/

  /*--------------------------------------------------------*/
  int *f1ptr = NULL, *f2ptr = NULL, *f3ptr = NULL;
  int f1val, f2val;
  int *endf1ptr = NULL, *endf2ptr = NULL;
  /*--------------------------------------------------------*/
  if ( strcmp(op, "intersection") == 0 ) {
    /* f1 must be sorted ascending  */
    if ( f1_rec.sort_type != ascending ) { go_BYE(-1); }
    if ( f2_rec.sort_type != ascending ) { go_BYE(-1); }
    /* Have not implemented case where f1 has null field  */
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( nn_f2_id >= 0 ) { go_BYE(-1); }
    /* Have implemented only for int  */
    if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
    if ( f2_rec.fldtype != I4 ) { go_BYE(-1); }

    nR3_allocated = min(nR1 , nR2);
    f3_type = f1_rec.fldtype;
    status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR3_allocated * f3_fldsz), &ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);
   
    nR3 = 0;
    f1ptr = (int *)f1_X; endf1ptr = f1ptr + nR1;
    f2ptr = (int *)f2_X; endf2ptr = f2ptr + nR2;
    f3ptr = (int *)f3_X; 
    for ( ; ; ) {
      if ( f1ptr == endf1ptr ) { break; }
      if ( f2ptr == endf2ptr ) { break; }
      f1val = *f1ptr;
      f2val = *f2ptr;
      if ( f1val == f2val ) { 
	*f3ptr = f1val;
	f3ptr++;
	nR3++;
	if ( nR3 > (nR1+nR2) ) { go_BYE(-1); }
	f1ptr++;
	f2ptr++;
      }
      else if ( f1val < f2val ) { 
	f1ptr++;
      }
      else if ( f2val < f1val ) { 
	f2ptr++;
      }
    }
    rs_munmap(f3_X, f3_nX);
    chdir(g_data_dir);
    if ( nR3 == 0 ) { 
      unlink(opfile);
    }
    else {
      if ( nR3 < nR3_allocated ) { 
        status = q_trunc(ddir_id, opfile, (nR3 * f3_fldsz)); cBYE(status);
      }
      else if ( nR3 > nR3_allocated ) { 
	go_BYE(-1);
      }
    }
    chdir(g_cwd);
  }
  else if ( strcmp(op, "a_minus_b") == 0 ) {
    /* f1, f2 must be sorted ascending  */
    if ( f1_rec.sort_type != ascending ) { go_BYE(-1); }
    if ( f2_rec.sort_type != ascending ) { go_BYE(-1); }
    /* Have not implemented case where f1, f2 has null field  */
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( nn_f2_id >= 0 ) { go_BYE(-1); }
    /* Have implemented only for int  */
    if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
    if ( f2_rec.fldtype != I4 ) { go_BYE(-1); }

    nR3_allocated = nR1;
    f3_type = f1_rec.fldtype;
    status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR3_allocated * f3_fldsz), &ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);

    nR3 = 0;
    f1ptr = (int *)f1_X; endf1ptr = f1ptr + nR1;
    f2ptr = (int *)f2_X; endf2ptr = f2ptr + nR2;
    f3ptr = (int *)f3_X; 
    for ( ; ( ( f1ptr != endf1ptr ) || ( f2ptr != endf2ptr ) ); ) {
      if ( f2ptr == endf2ptr ) { /* copy items from f1 */
        f1val = *f1ptr;
	*f3ptr = f1val;
	f3ptr++;
	f1ptr++;
	nR3++;
      }
      else if ( f1ptr == endf1ptr ) { /* nothing more to do */
	break; 
      }
      else {
	f1val = *f1ptr;
	f2val = *f2ptr;
	if ( f1val < f2val ) { 
	  *f3ptr = f1val;
	  f1ptr++;
	  f3ptr++;
	  nR3++;
	}
	else if ( f1val == f2val ) { 
	  /* Do not copy this value into A since it is in B */
	  f1ptr++;
	  f2ptr++;
	}
	else { /* advance pointer over this element */
	  f2ptr++;
	}
      }
    }
    rs_munmap(f3_X, f3_nX);
    status = q_trunc(ddir_id, opfile, (nR3 * f3_fldsz)); cBYE(status);
  }
  else if ( ( strcmp(op, "union") == 0 ) || 
            ( strcmp(op, "pvalcalc") == 0 ) ) {
    /* f1, f2 must be sorted ascending  */
    if ( f1_rec.sort_type != ascending ) { go_BYE(-1); }
    if ( f2_rec.sort_type != ascending ) { go_BYE(-1); }
    /* Have not implemented case where f1 has null field  */
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( nn_f2_id >= 0 ) { go_BYE(-1); }
    if ( f1_rec.fldtype != f2_rec.fldtype ) { go_BYE(-1); }

    nR3_allocated = nR1 + nR2;
    f3_type = f1_rec.fldtype;
    status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);
    filesz = f3_fldsz * nR3_allocated;
    status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);

    long long mask;
    if ( strcmp(op, "pvalcalc") == 0 ) {
      bool is_null = false;
      // TODO P1 Switch extract_i8 to extract_UI8 
      status = extract_I8(options, "mask=[", "]", &mask, &is_null);
      if ( is_null ) { go_BYE(-1); }
    }
    else {
      mask = 0;
    }
    switch ( f1_rec.fldtype ) { 
      case I4 : 
        status = union_I4((int *)f1_X, nR1, (int *)f2_X, nR2, 
           (int *)f3_X, &nR3, (unsigned int)mask);
        cBYE(status);
      break;
      case I8 : 
        status = union_I8((long long *)f1_X, nR1, (long long *)f2_X, nR2, 
          (long long *)f3_X, &nR3, (unsigned long long)mask);
      cBYE(status);
      break;
      default :
        go_BYE(-1);
      break;
    }
    rs_munmap(f3_X, f3_nX);
    chdir(g_data_dir);
    truncate(opfile, (nR3 * f3_fldsz));
    chdir(g_cwd);
  }
  else if (( strcmp(op, "pos_copy") == 0 ) || 
	   ( strcmp(op, "pos_sel_copy") == 0 ) ) {

    nR3_allocated = nR3 = nR2;
    f3_type = f1_rec.fldtype;
    status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR3 * f3_fldsz), &ddir_id); cBYE(status);
    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);
    long long cnt = LLONG_MIN;
    bool is_null_vals = false;
    if ( ( nn_f1_id < 0 ) || ( nn_f2_id < 0 ) ) { is_null_vals = true; }
    long long *f1I8 = (long long *)f1_X;
    long long *f2I8 = (long long *)f2_X;
    long long *f3I8 = (long long *)f3_X;
    char *f1I1 = (char *)f1_X;

    int   *f1I4 = (int *) f1_X;
    int   *f2I4 = (int *) f2_X;
    int   *f3I4 = (int *) f3_X;
    char  *f3I1 = (char *)f3_X;
    /*
      fprintf(stderr, "nn_f1_id = %d \n", nn_f1_id);
      fprintf(stderr, "nn_f2_id = %d \n", nn_f2_id);
      fprintf(stderr, "is_null_vals = %d \n", is_null_vals);
    */

    if ( strcmp(op, "pos_copy") == 0 ) { 
      if ( is_null_vals == false ) {
	cnt = nR3;
	switch ( f1_rec.fldtype ) {
	case I4 : 
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_copy_I4_I4(f1I4, nR1, f2I4, nR2, f3I4);
	    break;
	  case I8 : 
	    status = pos_copy_I4_I8(f1I4, nR1, f2I8, nR2, f3I4);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I8 : 
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_copy_I8_I4(f1I8, nR1, f2I4, nR2, f3I8);
	    break;
	  case I8 : 
	    status = pos_copy_I8_I8(f1I8, nR1, f2I8, nR2, f3I8);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I1 : case I2 : case F4: case F8: default : 
	  go_BYE(-1);
	  break;
	}
      }
      else {
	cnt = 0;
	switch ( f1_rec.fldtype ) {
	case I4 : 
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_nn_copy_I4_I4(f1I4, nn_f1_X, nR1, f2I4, nn_f2_X, 
				       nR2, f3I4, &cnt);
	    break;
	  case I8 : 
	    status = pos_nn_copy_I4_I8(f1I4, nn_f1_X, nR1, f2I8, nn_f2_X, 
				       nR2, f3I4, &cnt);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I8 : 
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_nn_copy_I8_I4(f1I8, nn_f1_X, nR1, f2I4, nn_f2_X, 
				       nR2, f3I8, &cnt);
	    break;
	  case I8 : 
	    status = pos_nn_copy_I8_I8(f1I8, nn_f1_X, nR1, f2I8, nn_f2_X, 
				       nR2, f3I8, &cnt);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I1 : 
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_nn_copy_I1_I4(f1I1, nn_f1_X, nR1, f2I4, nn_f2_X, 
				       nR2, f3I1, &cnt);
	    break;
	  case I8 : 
	    status = pos_nn_copy_I1_I8(f1I1, nn_f1_X, nR1, f2I8, nn_f2_X, 
				       nR2, f3I1, &cnt);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I2 : case F4: case F8: default : 
	  go_BYE(-1);
	  break;
	}
      }
    }
    else if ( strcmp(op, "pos_sel_copy") == 0 ) {

      bool is_lb_null, is_ub_null, is_sel_fld_null;
      long long lbI8 = LLONG_MIN, ubI8 = LLONG_MAX; 
      int lbI4 = INT_MIN, ubI4 = INT_MAX;
      char sel_fld[MAX_LEN_FLD_NAME+1];
      zero_string(sel_fld, (MAX_LEN_FLD_NAME+1));
      if ( ( options == NULL ) || ( *options == '\0' ) ) { go_BYE(-1); }
      status = extract_I8(options, "lb=[", "]", &lbI8, &is_lb_null);
      status = extract_I8(options, "ub=[", "]", &ubI8, &is_ub_null);
      status = extract_S(options, "sel_fld=[", "]", sel_fld, MAX_LEN_FLD_NAME,
			 &is_sel_fld_null);
      if ( ( is_lb_null ) && ( is_ub_null ) ) { go_BYE(-1); }
      if ( ( is_lb_null == false ) && ( is_ub_null == false ) ) { 
        if ( lbI8 > ubI8 ) { go_BYE(-1); }
      }
      /*--------------------------------------------------------*/
      switch ( f1_rec.fldtype ) {
      case I4 : 
        status = extract_I4(options, "lb=[", "]", &lbI4, &is_lb_null);
        status = extract_I4(options, "ub=[", "]", &ubI4, &is_ub_null);
	if ( is_lb_null ) { lbI4 = INT_MIN; }
	if ( is_ub_null ) { ubI4 = INT_MAX; }
	break;
      case I8 : 
	if ( is_lb_null ) { lbI8 = LLONG_MIN; }
	if ( is_ub_null ) { ubI8 = LLONG_MAX; }
	break;
      default :
	go_BYE(-1);
	break;
      }
      /*--------------------------------------------------------*/
      if ( is_sel_fld_null ) {
	switch ( f1_rec.fldtype ) {
	case I4 : 
	  if ( is_lb_null ) { lbI4 = INT_MIN; }
	  if ( is_ub_null ) { ubI4 = INT_MAX; }
	  if ( lbI8 < INT_MIN ) { go_BYE(-1); } else { lbI4 = lbI8; }
	  if ( ubI8 > INT_MAX ) { go_BYE(-1); } else { ubI4 = ubI8; }
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_sel_copy_I4_I4(f1I4, nR1, f2I4, nR2, lbI4, ubI4, f3I4, &cnt);
	    break;
	  case I8 : 
	    status = pos_sel_copy_I4_I8(f1I4, nR1, f2I8, nR2, lbI4, ubI4, f3I4, &cnt);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I8 : 
	  if ( is_lb_null ) { lbI8 = LLONG_MIN; }
	  if ( is_ub_null ) { ubI8 = LLONG_MAX; }
	  switch ( f2_rec.fldtype ) { 
	  case I4 : 
	    status = pos_sel_copy_I8_I4(f1I8, nR1, f2I4, nR2, lbI8, ubI8, f3I8, &cnt);
	    break;
	  case I8 : 
	    status = pos_sel_copy_I8_I8(f1I8, nR1, f2I8, nR2,  lbI8, ubI8, f3I8, &cnt);
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	  break;
	case I1 : case I2 : case F4: case F8: default : 
	  go_BYE(-1);
	  break;
	}
	/*--------------------------------------------------------*/
      }
      else {
        status = is_fld(NULL, t1_id, sel_fld, &sel_fld_id, &sel_fld_rec, 
			&nn_sel_fld_id, &nn_sel_fld_rec); 
        cBYE(status);
        chk_range(sel_fld_id, 0, g_n_fld);
        status = get_data(sel_fld_rec, &sel_fld_X, &sel_fld_nX, false); 
	cBYE(status);
        if ( nn_sel_fld_id >= 0 ) { 
          status = get_data(nn_sel_fld_rec, &nn_sel_fld_X, &nn_sel_fld_nX, false); 
	  cBYE(status);
	}
	int *selI4 = (int *)sel_fld_X;
	long long *selI8 = (long long *)sel_fld_X;
	/*--------------------------------------------------------*/
	cnt = 0;
	if ( is_null_vals == false ) {

	  switch ( f1_rec.fldtype ) {
	  case I4 : 
	    switch ( f2_rec.fldtype ) { 
	    case I4 : 
	      status = pos_sel_aux_copy_I4_I4(f1I4, selI4, nR1, f2I4, nR2, 
					      lbI4, ubI4, f3I4, &cnt);
	      break;
	    case I8 : 
	      status = pos_sel_aux_copy_I4_I8(f1I4, selI4, nR1, f2I8, nR2, 
					      lbI4, ubI4, f3I4, &cnt);
	      break;
	    case I1 : case I2 : case F4: case F8: default : 
	      go_BYE(-1);
	      break;
	    }
	    break;
	  case I8 : 
	    switch ( f2_rec.fldtype ) { 
	    case I4 : 
	      status = pos_sel_aux_copy_I8_I4(f1I8, selI8, nR1, f2I4, nR2, 
					      lbI4, ubI4, f3I8, &cnt);
	      break;
	    case I8 : 
	      status = pos_sel_aux_copy_I8_I8(f1I8, selI8, nR1, f2I8, nR2, 
					      lbI4, ubI4, f3I8, &cnt);
	      break;
	    case I1 : case I2 : case F4: case F8: default : 
	      go_BYE(-1);
	      break;
	    }
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	}
	else {
	  switch ( f1_rec.fldtype ) {
	  case I4 : 
	    if ( lbI4 < INT_MIN ) { go_BYE(-1); }
	    if ( ubI4 > INT_MAX ) { go_BYE(-1); }
	    switch ( f2_rec.fldtype ) { 
	    case I4 : 
	      status = pos_sel_aux_nn_copy_I4_I4(
						 f1I4, nn_f1_X, selI4, (char *)NULL, nR1, f2I4, nn_f2_X, nR2, 
						 lbI4, ubI4, f3I4, &cnt);
	      break;
	    case I8 : 
	      status = pos_sel_aux_nn_copy_I4_I8(
						 f1I4, nn_f1_X, selI4, (char *)NULL, nR1, f2I8, nn_f2_X, nR2, 
						 lbI4, ubI4, f3I4, &cnt);
	      break;
	    case I1 : case I2 : case F4: case F8: default : 
	      go_BYE(-1);
	      break;
	    }
	    break;
	  case I8 : 
	    switch ( f2_rec.fldtype ) { 
	    case I4 : 
	      status = pos_sel_aux_nn_copy_I8_I4(
						 f1I8, nn_f1_X, selI8, (char *)NULL, nR1, f2I4, nn_f2_X, nR2, 
						 lbI4, ubI4, f3I8, &cnt);
	      break;
	    case I8 : 
	      status = pos_sel_aux_nn_copy_I8_I8(
						 f1I8, nn_f1_X, selI8, (char *)NULL, nR1, f2I8, nn_f2_X, nR2, 
						 lbI4, ubI4, f3I8, &cnt);
	      break;
	    case I1 : case I2 : case F4: case F8: default : 
	      go_BYE(-1);
	      break;
	    }
	    break;
	  case I1 : case I2 : case F4: case F8: default : 
	    go_BYE(-1);
	    break;
	  }
	}
	/*--------------------------------------------------------*/
      }
    }
    rs_munmap(f3_X, f3_nX);
    if ( cnt == LLONG_MIN ) { go_BYE(-1); }
    if ( cnt < nR3 ) {
      nR3 = cnt;
      chdir(g_data_dir);
      if ( cnt == 0 ) { 
	unlink(opfile);
        zero_string(opfile, (MAX_LEN_FILE_NAME+1));
      }
      else {
	status = q_trunc(ddir_id, opfile, (f3_fldsz * nR3)); cBYE(status);
      }
      chdir(g_cwd);
    }
    sprintf(rslt_buf,"%lld\n", nR3);
  }
  else if ( strcmp(op, "pos_count") == 0 ) {
    bool is_bitvec = false;
    if ( ( options != NULL ) && ( *options != '\0' ) ) {
      if ( strcmp(options, "bitvec") == 0 ) {
	is_bitvec = true;
      }
      else { go_BYE(-1); }
    }
    /* Have not implemented case where f1 has null field  */
    if ( nn_f1_id >= 0 ) { go_BYE(-1); }
    if ( nn_f2_id >= 0 ) { go_BYE(-1); }
    status = is_tbl(t3, &t3_id, &t3_rec); cBYE(status);
    chk_range(t3_id, 0, g_n_tbl);
    nR3 = g_tbls[t3_id].nR;
    /*-------------------------------------------------------- */
    f3_type = I4; 
    status = get_fld_sz(f3_type, &f3_fldsz); cBYE(status);
    status = mk_temp_file(opfile, (nR3 * f3_fldsz), &ddir_id); cBYE(status);

    status = q_mmap(ddir_id, opfile, &f3_X, &f3_nX, true); cBYE(status);

#ifdef IPP
    ippsZero_32s((int *)f3_X, nR3);
#else
    assign_const_I4((int *)f3_X, nR3, 0);
#endif

    switch ( f1_rec.fldtype ) { 
    case I4 : 
      switch ( f2_rec.fldtype ) { 
      case I4 : 
	if ( is_bitvec ) { 
	status = pos_count_bitvec_I4_I4((int *)f1_X, nR1, (int *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	else {
	status = pos_count_I4_I4((int *)f1_X, nR1, (int *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	break;
      case I8 : 
	if ( is_bitvec ) { 
	status = pos_count_bitvec_I4_I8((int *)f1_X, nR1, (long long *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	else {
	status = pos_count_I4_I8((int *)f1_X, nR1, (long long *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	break;
      case I1 : case I2 : case F4: case F8: default : 
	go_BYE(-1);
	break;
      }
      break;
    case I8 : 
      switch ( f2_rec.fldtype ) { 
      case I4 : 
	if ( is_bitvec ) { 
	status = pos_count_I8_I4((long long *)f1_X, nR1, (int *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	else {
	status = pos_count_I8_I4((long long *)f1_X, nR1, (int *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	break;
      case I8 : 
	if ( is_bitvec ) { 
	status = pos_count_I8_I8((long long *)f1_X, nR1, (long long *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	else {
	status = pos_count_I8_I8((long long *)f1_X, nR1, (long long *)f2_X, nR2, 
				 (int *)f3_X, nR3);
	}
	break;
      case I1 : case I2 : case F4: case F8: default : 
	go_BYE(-1);
	break;
      }
      break;
    case I1 : case I2 : case F4: case F8: default : 
      go_BYE(-1);
      break;
    }
  }
  else if ( strcmp(op, "pos_mmad") == 0 ) {
    int *i4minval = NULL, *i4maxval = NULL;
    long long *i8minval = NULL, *i8maxval = NULL;
    long long *sum = NULL, *cnt = NULL;
    long long block_size; int nT; 
    int max_num_chunks = g_num_cores * g_num_cores;
    status = partition(nR2, 1024, max_num_chunks, &block_size, &nT);
    cBYE(status);
    i4minval = malloc(max_num_chunks * sizeof(int));
    i4maxval = malloc(max_num_chunks * sizeof(int));
    i8minval = malloc(max_num_chunks * sizeof(long long));
    i8maxval = malloc(max_num_chunks * sizeof(long long));
    sum      = malloc(max_num_chunks * sizeof(long long));
    cnt      = malloc(max_num_chunks * sizeof(long long));
    for ( int tid = 0; tid < max_num_chunks; tid++ ) {
      i4minval[tid] = INT_MAX;
      i4maxval[tid] = INT_MIN;
      i8minval[tid] = LLONG_MAX;
      i8maxval[tid] = LLONG_MIN;
      sum[tid] = 0;
      cnt[tid] = 0;
    }
    /*--------------------------------------------------------*/
    for ( int tid = 0; tid < nT; tid++ ) {
      long long lb = 0 + (tid * block_size);
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = nR2; }
      long long nX = (ub -lb);

      int *I4f2 = NULL; long long *I8f2 = NULL; char *nnf2 = NULL;
      switch ( f1_rec.fldtype ) { 
      case I4 : 
	switch ( f2_rec.fldtype ) { 
	case I4 : 
	  I4f2 = (int *)f2_X; I4f2 += lb;
	  if ( ( nn_f1_id >= 0 ) || ( nn_f2_id >= 0 ) ) { 
	    nnf2 = nn_f2_X;    nnf2 += lb;
	    status = pos_mmad_I4_I4_nn( 
				       (int *)f1_X, nn_f1_X, nR1, 
				       I4f2, nnf2, nR2, 
				       &(i4minval[tid]), &(i4maxval[tid]), &(sum[tid]), &(cnt[tid]));
	  }
	  else {
	    status = pos_mmad_I4_I4( (int *)f1_X, nR1, 
				     I4f2, nX, 
				     &(i4minval[tid]), &(i4maxval[tid]), &(sum[tid]));
	  }
	  break;
	case I8 : 
	  I8f2 = (long long *)f2_X; I8f2 += lb;
	  if ( ( nn_f1_id >= 0 ) || ( nn_f2_id >= 0 ) ) { 
	    nnf2 = nn_f2_X;    nnf2 += lb;
	    status = pos_mmad_I4_I8_nn( 
				       (int *)f1_X, nn_f1_X, nR1, 
				       I8f2, nnf2, nX, 
				       &(i4minval[tid]), &(i4maxval[tid]), &(sum[tid]), &(cnt[tid]));
	  }
	  else {
	    status = pos_mmad_I4_I8( 
				    (int *)f1_X, nR1, 
				    I8f2, nX,
				    &(i4minval[tid]), &(i4maxval[tid]), &(sum[tid]));
	  }
	  break;
	case I1 : case I2 : case F4: case F8: default : 
	  go_BYE(-1);
	  break;
	}
	// XXX sprintf(rslt_buf,"%d:%d:%lf\n", i4minval, i4maxval, sum/nR2);
	break;
      case I8 : 
	switch ( f2_rec.fldtype ) { 
	case I4 : 
	  I8f2 = (long long *)f2_X; I4f2 += lb;
	  if ( ( nn_f1_id >= 0 ) || ( nn_f2_id >= 0 ) ) { 
	    nnf2 = nn_f2_X;    nnf2 += lb;
	    status = pos_mmad_I8_I4_nn( 
				       (long long *)f1_X, nn_f1_X, nR1, 
				       I4f2, nnf2, nX, 
				       &(i8minval[tid]), &(i8maxval[tid]), &(sum[tid]), &(cnt[tid]));
	  }
	  else {
	    status = pos_mmad_I8_I4(
				    (long long *)f1_X, nR1, I4f2, nX,
				    &(i8minval[tid]), &(i8maxval[tid]), &(sum[tid]));
	  }
	  break;
	case I8 : 
	  if ( ( nn_f1_id >= 0 ) || ( nn_f2_id >= 0 ) ) { 
	    nnf2 = nn_f2_X;    nnf2 += lb;
	    status = pos_mmad_I8_I8_nn( 
				       (long long *)f1_X, nn_f1_X, nR1, 
				       I8f2, nnf2, nX, 
				       &(i8minval[tid]), &(i8maxval[tid]), &(sum[tid]), &(cnt[tid]));
	  }
	  else {
	    status = pos_mmad_I8_I8(
				    (long long *)f1_X, nR1, I8f2, nX,
				    &(i8minval[tid]), &(i8maxval[tid]), &(sum[tid]));
	  }
	  break;
	case I1 : case I2 : case F4: case F8: default : 
	  go_BYE(-1);
	  break;
	}
	// XXXX sprintf(rslt_buf,"%lld:%lld:%lf\n", i8minval, i8maxval, sum/nR2);
	break;
      case I1 : case I2 : case F4: case F8: default : 
	go_BYE(-1);
	break;
      }
    }
    // accumulate over the threads
    int final_i4minval = INT_MAX, final_i4maxval = INT_MIN;
    long long final_i8minval = LLONG_MAX, final_i8maxval = LLONG_MIN;
    long long final_sum = 0;
    for ( int tid = 0; tid < nT; tid++ ) { 
      final_sum += sum[tid];
      switch( f2_rec.fldtype ) { 
      case I4 : 
	if ( i4minval[tid] < final_i4minval ) {
	  final_i4minval = i4minval[tid];
	}
	if ( i4maxval[tid] < final_i4maxval ) {
	  final_i4maxval = i4maxval[tid];
	}
	break;
      case I8 : 
	if ( i8minval[tid] < final_i8minval ) {
	  final_i8minval = i8minval[tid];
	}
	if ( i8maxval[tid] < final_i8maxval ) {
	  final_i8maxval = i8maxval[tid];
	}
	break;
      case I1 : case I2 : case F4: case F8: default : 
	go_BYE(-1);
	break;
      }
    }
    free_if_non_null(i4minval);
    free_if_non_null(i4maxval);
    free_if_non_null(i8minval);
    free_if_non_null(i8maxval);
    free_if_non_null(sum     );
    free_if_non_null(cnt     );
    switch( f2_rec.fldtype ) { 
    case I4 : 
      sprintf(rslt_buf,"%d:%d:%lld:%lld\n", final_i4minval, 
	      final_i4maxval, final_sum, nR2);
      break;
    case I8 : 
      sprintf(rslt_buf,"%lld:%lld:%lld:%lld\n", final_i8minval, 
	      final_i8maxval, final_sum, nR2);
      break;
    case I1 : case I2 : case F4: case F8: default : 
      go_BYE(-1);
      break;
    }
  }
  else { go_BYE(-1); }
  rs_munmap(f3_X, f3_nX);
  if ( nR3 != nR3_allocated ) {
    if ( *opfile == '\0' ) {
      /* Means that there is no output. We have unlinked the opfile */
      fprintf(stderr, "DBG: No output produced\n");
      goto BYE;
    }
    else {
      status = q_trunc(ddir_id, opfile, (nR3 * f3_fldsz)); cBYE(status);
    }
  }

  if ( ( strcmp(op, "pos_count") != 0 ) && ( strcmp(op, "pos_mmad") != 0 ) ) {
    if ( nR3 == 0 ) { go_BYE(0); }
    char str_nR3[32]; sprintf(str_nR3, "%lld", nR3);
    status = add_tbl(t3, str_nR3, &t3_id, &t3_rec); cBYE(status);
  }
  if ( strcmp(op, "pos_mmad") == 0 ) {
  }
  else {
    /* Add count field to meta data  */
    status = add_fld(t3_id, f3, ddir_id, opfile, f3_type, -1, &f3_id, &f3_rec); cBYE(status);
    /* Since f1 and f2 are sorted ascending, so musyt f3 be  */
    if ( ( strcmp(op, "union") == 0 ) || ( strcmp(op, "intersection") == 0 ) || 
         ( strcmp(op, "a_minus_b") == 0 ) ) {
      status = int_set_meta(t3_id, f3_id, "sort_type", "ascending"); cBYE(status);
    }
  }
  /*-----------------------------------------------------------*/
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  rs_munmap(f3_X, f3_nX);
  rs_munmap(sel_fld_X, sel_fld_nX);
  rs_munmap(nn_sel_fld_X, nn_sel_fld_nX);
  return(status);
}
