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
#include "mk_temp_file.h"
#include "vec_f1s1opf2.h"
#include "get_type_op_fld.h"

#include "add_scalar_I2.h"
#include "sub_scalar_I2.h"
#include "mul_scalar_I2.h"
#include "div_scalar_I2.h"
#include "rem_scalar_I2.h"

#include "add_scalar_I4.h"
#include "sub_scalar_I4.h"
#include "mul_scalar_I4.h"
#include "div_scalar_I4.h"
#include "rem_scalar_I4.h"

#include "add_scalar_I8.h"
#include "sub_scalar_I8.h"
#include "mul_scalar_I8.h"
#include "div_scalar_I8.h"
#include "rem_scalar_I8.h"

#include "add_scalar_F4.h"
#include "sub_scalar_F4.h"
#include "mul_scalar_F4.h"
#include "div_scalar_F4.h"

#include "add_scalar_F8.h"
#include "sub_scalar_F8.h"
#include "mul_scalar_F8.h"
#include "div_scalar_F8.h"

#include "and_scalar_I1.h"
#include "or_scalar_I1.h"
#include "bwise_and_scalar_I4.h"
#include "bwise_and_scalar_I8.h"
#include "bwise_and_scalar_I1.h"
#include "bwise_or_scalar_I4.h"
#include "bwise_or_scalar_I8.h"
#include "bwise_or_scalar_I1.h"
#include "bwise_xor_scalar_I4.h"
#include "bwise_xor_scalar_I8.h"
#include "bwise_xor_scalar_I1.h"

#include "shift_left_scalar_I4.h"
#include "shift_left_scalar_I8.h"
#include "shift_right_scalar_UI4.h"
#include "shift_right_scalar_UI8.h"

#include "cmp_le_scalar_I4.h"
#include "cmp_ge_scalar_I4.h" 
#include "cmp_eq_scalar_I4.h"
#include "cmp_ne_scalar_I4.h"
#include "cmp_gt_scalar_I4.h"
#include "cmp_lt_scalar_I4.h"
#include "cmp_le_scalar_I8.h"
#include "cmp_ge_scalar_I8.h"
#include "cmp_eq_scalar_I8.h"
#include "cmp_ne_scalar_I8.h"
#include "cmp_lt_scalar_I8.h"
#include "cmp_gt_scalar_I8.h"
#include "cmp_le_scalar_F4.h" 
#include "cmp_ge_scalar_F4.h"
#include "cmp_eq_scalar_F4.h"
#include "cmp_ne_scalar_F4.h"
#include "cmp_lt_scalar_F4.h"
#include "cmp_gt_scalar_F4.h"
#include "cmp_le_scalar_I1.h"
#include "cmp_ge_scalar_I1.h"
#include "cmp_eq_scalar_I1.h"
#include "cmp_ne_scalar_I1.h"
#include "cmp_lt_scalar_I1.h"
#include "cmp_gt_scalar_I1.h"

#include "cmp_eq_scalar_I2.h"

#include "cmp_eq_mult_scalar_I1.h"
#include "cmp_eq_mult_scalar_I2.h"
#include "cmp_eq_mult_scalar_I4.h"
#include "cmp_eq_mult_scalar_I8.h"

#include "cmp_lt_or_gt_I4.h"
#include "cmp_le_or_ge_I4.h"
#include "cmp_gt_and_lt_I4.h"
#include "cmp_ge_and_le_I4.h"

#include "f1opf2_cum.h"
#include "cmp_eq_scalar_char_string.h"

#include "assign_I1.h"
#include "sort_asc_I8.h"
#include "sort_asc_F8.h"

/*---------------------------------------------------------------*/
/* START FUNC DECL  */
int 
vec_f1s1opf2(
	     long long nR,
	     FLD_TYPE f1_fldtype,
	     char *f1_X,
	     char *in_nn_f1_X,
	     char *str_scalar,
	     char *op,
	     int *ptr_ddir_id, 
	     char opfile[MAX_LEN_FILE_NAME+1],
	     int *ptr_nn_ddir_id, 
	     char nn_opfile[MAX_LEN_FILE_NAME+1],
	     FLD_TYPE *ptr_f2_fld_type
	     )
/* STOP FUNC DECL  */
{
  int status = 0;
  FLD_TYPE op_fldtype;
  char *op_X = NULL; size_t n_op_X = 0; /* for mmaped output file */
  char *nn_op_X = NULL; size_t n_nn_op_X = 0; /* for mmaped nn file */
  int nT; long long block_size;
  long long svals_I8[MAX_NUM_SCALARS]; 
  int       svals_I4[MAX_NUM_SCALARS]; 
  short     svals_I2[MAX_NUM_SCALARS]; 
  char      svals_I1[MAX_NUM_SCALARS]; 
  float     svals_F4[MAX_NUM_SCALARS]; 
  double    svals_F8[MAX_NUM_SCALARS]; 
  long long sval_I8 = LLONG_MAX;
  int       sval_I4 = INT_MAX;
  short     sval_I2 = SHRT_MAX;
  char      sval_I1 = SCHAR_MAX;
  float     sval_F4 = FLT_MAX;
  double    sval_F8 = DBL_MAX;
  bool is_debug = false; long long *part_cnt = NULL;
  int num_scalar_vals = -1;

  status =  break_into_scalars(str_scalar, f1_fldtype, &num_scalar_vals,
	&sval_I8, &sval_I4, &sval_I2, &sval_I1, &sval_F4, &sval_F8,
	svals_I8, svals_I4, svals_I2, svals_I1, svals_F4, svals_F8);
  cBYE(status);
  /*-------------------------------------------- */
  status = partition(nR, 8192, -1, &block_size, &nT); cBYE(status);
  if ( is_debug ) { 
    part_cnt = malloc(nT * sizeof(long long));
    return_if_malloc_failed(part_cnt);
  }
  // fprintf(stderr, "Number of partitions = %d   \n", nT);
  // fprintf(stderr, "block_size           = %lld \n", block_size);
  /*-------------------------------------------- */
  status = get_type_op_fld("f1s1opf2", op, f1_fldtype, f1_fldtype, "", &op_fldtype);
  cBYE(status);
  int fldsz = INT_MAX; 
  *ptr_ddir_id = INT_MAX; 
  *ptr_nn_ddir_id = INT_MAX;
  status = get_fld_sz(op_fldtype, &fldsz);
  /*------------------------------------------ */
  /* TODO: P2: Storage should be allocated external to this function */
  status = mk_temp_file(opfile, (fldsz * nR), ptr_ddir_id); cBYE(status);
  status = q_mmap(*ptr_ddir_id, opfile, &op_X, &n_op_X, true); cBYE(status);
  /*--------------------------------------------*/
  if ( in_nn_f1_X == NULL ) {
    /* No need for nn file */
  }
  else {
    status = mk_temp_file(nn_opfile, nR * sizeof(char), ptr_nn_ddir_id); 
    cBYE(status);
    status = q_mmap(*ptr_nn_ddir_id, nn_opfile, &nn_op_X, &n_nn_op_X, true); 
    cBYE(status);
  }
  /*--------------------------------------------*/
  // fprintf(stderr, "block_size=%lld, num_threads=%d\n", block_size, nT);
  for ( int tid = 0; tid < nT; tid++ ) {

    long long lb = (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = ub - lb; 
    // fprintf(stderr, "%d,%lld,%lld\n", tid,lb, ub);

    if ( is_debug ) { part_cnt[tid] = nX; }
    char *t_in_X = NULL; char *t_op_X = NULL;
    char                 *opI1 = NULL; 
    short  *f1I2 = NULL, *opI2 = NULL;
    int    *f1I4 = NULL, *opI4 = NULL;
    float  *f1F4 = NULL, *opF4 = NULL;
    double *f1F8 = NULL, *opF8 = NULL;
    unsigned int    *f1UI4 = NULL, *opUI4 = NULL;
    // Handle  nn field if needed 
    if ( nn_op_X != NULL ) { 
      assign_I1(nn_op_X+lb, in_nn_f1_X+lb, nX); 
    }
    if ( f1_fldtype == I2 ) {
      f1I2 = (short *)f1_X; f1I2 += lb;
      opI2 = (short *)op_X; opI2 += lb;
      opI1 = (char  *)op_X; opI1 += lb;
      if ( strcmp(op, "==") == 0 ) {
	if ( num_scalar_vals == 1 ) { 
	  cmp_eq_scalar_I2(f1I2, sval_I2, nX, opI1);
	}
	else {
	  cmp_eq_mult_scalar_I2(f1I2, nX, svals_I2, num_scalar_vals, opI1);
	}
      }
      else if ( strcmp(op, "+") == 0 ) {
	add_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I2(f1I2, sval_I2, nX, opI2);
      }
      else { if ( status < 0 ) { WHEREAMI; } status = -1; continue; }
    }
    else if ( f1_fldtype == I4 ) {
      f1I4  = (int          *)f1_X; f1I4  += lb;
      f1UI4 = (unsigned int *)f1_X; f1UI4 += lb;
      opI4  = (int          *)op_X; opI4 += lb;
      opI1  = (char         *)op_X; opI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "<<") == 0 ) {
	shift_left_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, ">>") == 0 ) {
	shift_right_scalar_UI4(f1UI4, sval_I4, nX, opUI4);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<||>") == 0 ) {
	if ( num_scalar_vals == 2 ) { 
	  cmp_lt_or_gt_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "<=||>=") == 0 ) {
	if ( num_scalar_vals == 2 ) { 
	  cmp_le_or_ge_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">&&<") == 0 ) {
	if ( num_scalar_vals == 2 ) { 
	  cmp_gt_and_lt_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, ">=&&<=") == 0 ) {
	if ( num_scalar_vals == 2 ) { 
	  cmp_ge_and_le_I4(f1I4, nX, svals_I4[0], svals_I4[1], opI1);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	}
      }
      else if ( strcmp(op, "==") == 0 ) {
	if ( num_scalar_vals == 1 ) { 
	  cmp_eq_scalar_I4(f1I4, sval_I4, nX, opI1);
	}
	else {
	  cmp_eq_mult_scalar_I4(f1I4, nX, svals_I4, num_scalar_vals, opI1);
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_scalar_I4(f1I4, sval_I4, nX, opI1);
      }
      else if ( strcmp(op, "&") == 0 ) {
	bwise_and_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor_scalar_I4(f1I4, sval_I4, nX, opI4);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
    }
    else if (f1_fldtype == I8 ) {
      t_in_X = (f1_X + ( sizeof(long long) * lb ));
      t_op_X = (op_X + ( sizeof(long long) * lb ));
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "<<") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	shift_left_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, ">>") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	shift_right_scalar_UI8((unsigned long long *)t_in_X, sval_I8, nX, (unsigned long long *)t_op_X);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ge_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_le_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "==") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_eq_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ne_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, ">") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_gt_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "<") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_lt_scalar_I8((long long *)t_in_X, sval_I8, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "&") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	bwise_and_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "|") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	bwise_or_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else if ( strcmp(op, "^") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	bwise_xor_scalar_I8((long long *)t_in_X, sval_I8, nX, (long long *)t_op_X);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
    }
    else if ( f1_fldtype == F4 ) {
      f1F4 = (float *)f1_X; f1F4 += lb;
      opF4 = (float *)op_X; opF4 += lb;
      opI1 = (char  *)op_X; opI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_F4(f1F4, sval_F4, nX, opF4);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_F4(f1F4, sval_F4, nX, opF4);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_F4(f1F4, sval_F4, nX, opF4);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_F4(f1F4, sval_F4, nX, opF4);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ge_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_le_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_eq_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ne_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_gt_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_lt_scalar_F4(f1F4, sval_F4, nX, opI1);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
    }
    else if ( f1_fldtype == F8 ) {
      f1F8 = (double *)f1_X; f1F8 += lb;
      opF8 = (double *)op_X; opF8 += lb;
      opI1 = (char  *)op_X; opI1 += lb;
      if ( strcmp(op, "+") == 0 ) {
	add_scalar_F8(f1F8, sval_F8, nX, opF8);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub_scalar_F8(f1F8, sval_F8, nX, opF8);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul_scalar_F8(f1F8, sval_F8, nX, opF8);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div_scalar_F8(f1F8, sval_F8, nX, opF8);
      }
      /* TODO P2 
      else if ( strcmp(op, ">=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ge_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_le_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_eq_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ne_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_gt_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_lt_scalar_F8(f1F8, sval_F8, nX, opI1);
      }
      */
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
      }
    }
    else if ( f1_fldtype == I1 ) {
      t_in_X = (f1_X + ( sizeof(bool) * lb ));
      t_op_X = (op_X + ( sizeof(bool) * lb ));
      if ( strcmp(op, "&") == 0 ) {
	bwise_and_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ge_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_le_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "==") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	if ( num_scalar_vals == 1 ) { 
	  cmp_eq_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
	}
	else {
	  cmp_eq_mult_scalar_I1((char *)t_in_X, nX, svals_I1, num_scalar_vals, (char *)t_op_X);
	}
      }
      else if ( strcmp(op, "!=") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_ne_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, ">") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_gt_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "<") == 0 ) {
	t_op_X = (op_X + ( sizeof(char) * lb ));
	cmp_lt_scalar_I1((char *)t_in_X, sval_I1, nX, (char *)t_op_X);
      }
      else { 
	status = -1; continue; 
      }
    }
    else {
      status = -1; continue;
    }
  }
  cBYE(status);
  if ( is_debug ) { 
    long long chk_nR = 0;
    for ( int i = 0; i < nT; i++ ) { 
      chk_nR += part_cnt[i];
    }
    if ( chk_nR != nR ) { 
    fprintf(stderr, "chk_nR = %lld \n", chk_nR);
    fprintf(stderr, "nR     = %lld \n", nR   );
    fprintf(stderr, "block_size     = %lld \n", block_size   );
    fprintf(stderr, "nT     = %d \n", nT   );
    fprintf(stderr, " -----\n");
    go_BYE(-1); }
  }
  /*--------------------------------------------*/
  *ptr_f2_fld_type = op_fldtype;
 BYE:
  free_if_non_null(part_cnt);
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  return(status);
}
