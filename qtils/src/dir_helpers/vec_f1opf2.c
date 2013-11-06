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
#include "vec_f1opf2.h"
#include "get_type_op_fld.h"
#include "extract_S.h"
#include "mk_temp_file.h"
#include "assign_I1.h"

#include "conv_I1_to_B.h"
#include "conv_B_to_I1.h"

#include "conv_F8_to_I4.h"
#include "conv_F8_to_I8.h"
#include "conv_F8_to_F4.h"

#include "conv_I4_to_I1.h"
#include "conv_I4_to_I2.h"
#include "conv_I4_to_I8.h"
#include "conv_I4_to_F4.h"
#include "conv_I4_to_F8.h"

#include "conv_I8_to_I4.h"
#include "conv_I8_to_I1.h"
#include "conv_I8_to_F4.h"
#include "conv_I8_to_F8.h"

#include "conv_I1_to_I4.h"
#include "conv_I1_to_I8.h"

#include "conv_I2_to_I4.h"
#include "conv_I2_to_I8.h"

#include "conv_F4_to_I4.h"
#include "conv_F4_to_I8.h"
#include "conv_F4_to_F8.h"

#include "ones_complement_I1.h"
#include "ones_complement_I4.h"
#include "ones_complement_I8.h"

#include "not_I1.h"
#include "not_I4.h"
#include "not_I8.h"

#include "incr_I4.h"
#include "incr_I8.h"

#include "decr_I4.h"
#include "decr_I8.h"


#include "bitcount_I4.h"
#include "bitcount_I8.h"

#include "vec_sqrt_F4.h"
#include "vec_sqrt_F8.h"
#include "vec_abs_F4.h"
#include "vec_abs_F8.h"
#include "vec_reciprocal_F4.h"
#include "vec_reciprocal_F8.h"
#include "vec_normal_cdf_inverse.h"
#include "vec_pval_from_zval.h"

//---------------------------------------------------------------
// START FUNC DECL 
int 
vec_f1opf2(
	   long long nR,
	   FLD_TYPE src_fldtype,
	   char *f1_X,
	   char *nn_f1_X,
	   char *op,
	   char *str_op_spec,
	   int *ptr_ddir_id,
	   char opfile[MAX_LEN_FILE_NAME+1],
	   int *ptr_nn_ddir_id,
	   char nn_opfile[MAX_LEN_FILE_NAME+1],
	   FLD_TYPE *ptr_dst_fldtype
	   )
// STOP FUNC DECL 
{
  int status = 0;

  char *op_X = NULL; size_t n_op_X = 0 ; /* for mmaped output file */
  char *nn_op_X = NULL; size_t n_nn_op_X = 0 ; /* for mmaped nn file */
  FLD_TYPE dst_fldtype;
#define BUFLEN 32
  char str_dst_fldtype[BUFLEN]; 
  bool is_null;
  int nT; long long block_size;
  long long filesz = -1, nn_filesz = -1;
  *ptr_ddir_id = INT_MAX;
  *ptr_nn_ddir_id = INT_MAX;
  /*---------------------------------------------*/
  
  status = partition(nR, 1024, -1, &block_size, &nT); cBYE(status);
  /* Get destination field type */
  if ( strcmp(op, "conv") == 0 ) {
    status = extract_S(str_op_spec, "newtype=[", "]", str_dst_fldtype, 
	BUFLEN, &is_null);
    cBYE(status);
    if ( is_null ) { go_BYE(-1); }
    status = unstr_fldtype(str_dst_fldtype, &dst_fldtype); cBYE(status);
  }
  else {
    status = get_type_op_fld("f1opf2", op, src_fldtype, src_fldtype, 
	str_op_spec, &dst_fldtype);
    cBYE(status);
  }
  //------------------------------------------
  if ( ( src_fldtype == I1 ) && ( dst_fldtype == B ) ) { 
    status = get_file_size_B(nR, &filesz); cBYE(status);
    nn_filesz = -1; // no nn field for B 
  }
  else if ( ( dst_fldtype == I1 ) && ( src_fldtype == B ) ) { 
    filesz = nR * sizeof(char); 
    nn_filesz = -1; // no nn field for B 
  }
  else {
    int fldsz = INT_MAX; 
    status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
    filesz    = nR * fldsz;
    nn_filesz = nR * sizeof(char);
  }
  status = mk_temp_file(opfile, filesz, ptr_ddir_id); cBYE(status);
  status = q_mmap(*ptr_ddir_id, opfile, &op_X, &n_op_X, 1); cBYE(status);
  //--------------------------------------------
  if ( ( nn_f1_X == NULL ) || ( dst_fldtype == B ) ) { 
    /* No need for nn file */
  }
  else {
    /* TODO: P2: Memory should be allocated outside this function */
    status = mk_temp_file(nn_opfile, nn_filesz, ptr_nn_ddir_id); cBYE(status);
    status = q_mmap(*ptr_nn_ddir_id, nn_opfile, &nn_op_X, &n_nn_op_X, 1); cBYE(status);
    assign_I1(nn_op_X, nn_f1_X, nR); cBYE(status);
  }
  /*--------------------------------------------*/
  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = (ub -lb);
    char *t_f1_X = NULL;
    char *t_op_X = NULL;
    float *f1F4 = NULL; float *opF4 = NULL;
    double *f1F8 = NULL, *opF8 = NULL;
    long long *f1I8 = NULL, *opI8 = NULL;
    int *f1I4 = NULL, *opI4 = NULL;
    char              *opI1 = NULL;
    if ( src_fldtype == F8 ) {
      f1F8 = (double    *)f1_X; f1F8 += lb;
      opF8 = (double    *)op_X; opF8 += lb;
      opI8 = (long long *)op_X; opI8 += lb;
      opI4 = (int       *)op_X; opI4 += lb;
      opF4 = (float     *)op_X; opF4 += lb;
      t_f1_X = (f1_X + ( sizeof(double) * lb ));
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F4 ) ) {
	t_op_X = (op_X + ( sizeof(float) * lb ));
	conv_F8_to_F4(f1F8, nX, opF4);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I8 ) ) { 
	conv_F8_to_I8(f1F8, nX, opI8);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I4 ) ) { 
	conv_F8_to_I4(f1F8, nX, opI4); 
      }
      else if ( strcmp(op, "sqrt") == 0 ) {
	vec_sqrt_F8(f1F8,  nX, opF8);
      }
      else if ( strcmp(op, "abs") == 0 ) {
	vec_abs_F8(f1F8, nX, opF8);
      }
      else if ( strcmp(op, "reciprocal") == 0 ) {
	vec_reciprocal_F8(f1F8, nX, opF8);
      }
      else if ( strcmp(op, "normal_cdf_inverse") == 0 ) {
	vec_normal_cdf_inverse(f1F8, nX, nn_op_X, opF8);
      }
      else if ( strcmp(op, "pval_from_zval") == 0 ) {
	vec_pval_from_zval(f1F8, nX, opF8);
      }
      else {
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
    }
    else if ( src_fldtype == I4 ) {
      t_f1_X = (f1_X + ( sizeof(int) * lb ));
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I1 ) ) { 
	t_op_X = (op_X + ( sizeof(char) * lb ));
	conv_I4_to_I1((int *)t_f1_X, nX, (char *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I2 ) ) { 
	t_op_X = (op_X + ( sizeof(short) * lb ));
	conv_I4_to_I2((int *)t_f1_X, nX, (short *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I8 ) ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	conv_I4_to_I8((int *)t_f1_X, nX, (long long *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F4 ) ) {
	t_op_X = (op_X + ( sizeof(float) * lb ));
	conv_I4_to_F4((int *)t_f1_X, nX, (float *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F8 ) ) {
	t_op_X = (op_X + ( sizeof(double) * lb ));
	conv_I4_to_F8((int *)t_f1_X, nX, (double *)t_op_X);
      }
      else if ( strcmp(op, "!") == 0 ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	not_I4((int *)t_f1_X, nX, (int *)t_op_X);
      }
      else if ( strcmp(op, "~") == 0 ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	ones_complement_I4((int *)t_f1_X, nX, (int *)t_op_X);
      }
      else if ( strcmp(op, "++") == 0 ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	incr_I4((int *)t_f1_X, nX, (int *)t_op_X);
      }
      else if ( strcmp(op, "--") == 0 ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	decr_I4((int *)t_f1_X, nX, (int *)t_op_X);
      }
      else if ( strcmp(op, "bitcount") == 0 ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	bitcount_I4((int *)t_f1_X, nX, (int *)t_op_X);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
    }
    else if ( src_fldtype == I8 ) {
      f1I8 = (long long *)f1_X; f1I8 += lb;
      opF8 = (double    *)op_X; opF8 += lb;
      opF4 = (float     *)op_X; opF4 += lb;
      opI8 = (long long *)op_X; opI8 += lb;
      opI4 = (int       *)op_X; opI4 += lb;
      opI1 = (char      *)op_X; opI1 += lb;
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I1 ) ) {
	conv_I8_to_I1(f1I8, nX, opI1);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I4 ) ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	conv_I8_to_I4(f1I8, nX, opI4);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F4 ) ) {
	t_op_X = (op_X + ( sizeof(float) * lb ));
	conv_I8_to_F4(f1I8, nX, opF4);
      }
      else if ( strcmp(op, "!") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	not_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "~") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	ones_complement_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "++") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	incr_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "--") == 0 ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	decr_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "bitcount") == 0 ) {
	bitcount_I8(f1I8, nX, opI4);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F8 ) ) {
	conv_I8_to_F8(f1I8, nX, opF8);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
    }
    else if ( src_fldtype == F4 ) {
      f1F4 = (float     *)f1_X; f1I4 += lb;
      opF4 = (float     *)op_X; opF4 += lb;
      opF8 = (double    *)op_X; opF8 += lb;
      opI8 = (long long *)op_X; opI8 += lb;
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I4 ) ) { 
	conv_F4_to_I4(f1F4, nX, opI4);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I8 ) ) {
	conv_F4_to_I8(f1F4, nX, opI8);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == F8 ) ) {
	conv_F4_to_F8(f1F4, nX, opF8);
      }
      else if ( strcmp(op, "sqrt") == 0 ) {
	t_op_X = (op_X + ( sizeof(float) * lb ));
	vec_sqrt_F4(f1F4, nX, opF4);
      }
      else if ( strcmp(op, "abs") == 0 ) {
	vec_abs_F4(f1F4, nX, opF4);
      }
      else if ( strcmp(op, "reciprocal") == 0 ) {
	vec_reciprocal_F4(f1F4, nX, opF4);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
    }
    else if ( src_fldtype == B ) {
      if ( ( ( lb / NUM_BITS_IN_I1 ) * NUM_BITS_IN_I1 ) != lb ) {
	WHEREAMI; status = -1; continue;
      }
      t_f1_X = f1_X;
      t_op_X = op_X;
      t_f1_X += (lb / NUM_BITS_IN_I1);
      t_op_X += lb;
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I1 ) ) {
	status = conv_B_to_I1((char *)t_f1_X, nX, (char *)t_op_X);
	if ( status < 0 ) { WHEREAMI; continue; }
      }
      else {
	if ( status == 0 ) { 
	  fprintf(stderr, "Invalid op = [%s] \n", op); WHEREAMI; 
	}
	status = -1; continue;
      }
    }
    else if ( src_fldtype == I2 ) {
      short *f1I2 = (short *)f1_X; f1_X += lb;
      int       *f2I4 = (int       *)op_X; op_X += lb;
      long long *f2I8 = (long long *)op_X; op_X += lb;
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I8 ) ) {
	conv_I2_to_I8(f1I2, nX, f2I8); 
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I4 ) ) {
	conv_I2_to_I4(f1I2, nX, f2I4); 
      }
      else {
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
    }
	
    else if ( src_fldtype == I1 ) {
      t_f1_X = (f1_X + ( sizeof(bool) * lb ));
      if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I8 ) ) {
	t_op_X = (op_X + ( sizeof(long long) * lb ));
	conv_I1_to_I8((char *)t_f1_X, nX, (long long *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == I4 ) ) {
	t_op_X = (op_X + ( sizeof(int) * lb ));
	conv_I1_to_I4((char *)t_f1_X, nX, (int *)t_op_X);
      }
      else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fldtype == B ) ) {
	if ( lb % NUM_BITS_IN_I1 != 0 ) { WHEREAMI; status = -1; continue; }
	t_op_X = op_X + (lb / NUM_BITS_IN_I1);
	status = conv_I1_to_B((char *)t_f1_X, nX, (char *)t_op_X);
	if ( status < 0  ) { WHEREAMI; status = -1; continue; }
      }
      else if ( strcmp(op, "!") == 0 ) {
	t_op_X = (op_X + ( sizeof(bool) * lb ));
	not_I1((char *)t_f1_X, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "~") == 0 ) {
	t_op_X = (op_X + ( sizeof(bool) * lb ));
	ones_complement_I1((char *)t_f1_X, nX, (char *)t_op_X);
      }
      else if ( strcmp(op, "!") == 0 ) {
	t_op_X = (op_X + ( sizeof(bool) * lb ));
	not_I1((char *)t_f1_X, nX, (char *)t_op_X);
      }
      else { 
	if ( status == 0 ) { 
	  fprintf(stderr, "Invalid op = [%s] \n", op); WHEREAMI; status = -1;
	}
      }
    }
    else { 
      WHEREAMI; status = -1; continue;
    }
  }
  cBYE(status);
  *ptr_dst_fldtype = dst_fldtype;
 BYE:
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  return(status);
}
