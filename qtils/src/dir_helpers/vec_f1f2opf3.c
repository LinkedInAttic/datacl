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
#include "vec_f1f2opf3.h"
#include "get_type_op_fld.h"
#include "mk_file.h"

#include "add2_I4.h"
#include "sub2_I4.h"
#include "mul2_I4.h"
#include "div2_I4.h"
#include "rem2_I4.h"
#include "shiftleft2_I4.h"
#include "shiftright2_UI4.h"

#include "add2_I8.h"
#include "sub2_I8.h"
#include "mul2_I8.h"
#include "div2_I8.h"
#include "rem2_I8.h"
#include "shiftleft2_I8.h"
#include "shiftright2_UI8.h"

#include "add2_F4.h"
#include "sub2_F4.h"
#include "mul2_F4.h"
#include "div2_F4.h"

#include "add2_F8.h"
#include "sub2_F8.h"
#include "mul2_F8.h"
#include "div2_F8.h"

#include "and2_I1.h"
#include "or2_I1.h"

#include "bwise_and2_I4.h"
#include "bwise_and2_I8.h"
#include "bwise_and2_I1.h"

#include "bwise_or2_I4.h"
#include "bwise_or2_I8.h"
#include "bwise_or2_I1.h"

#include "bwise_xor2_I4.h"
#include "bwise_xor2_I8.h"
#include "bwise_xor2_I1.h"

#include "bwise_a_and_not_b_I8.h"
#include "a_and_not_b_I1.h"

#include "cmp_ge_I1.h"
#include "cmp_le_I1.h"
#include "cmp_eq_I1.h"
#include "cmp_ne_I1.h"
#include "cmp_gt_I1.h"
#include "cmp_lt_I1.h"

#include "cmp_ge_I4.h"
#include "cmp_le_I4.h"
#include "cmp_eq_I4.h"
#include "cmp_ne_I4.h"
#include "cmp_gt_I4.h"
#include "cmp_lt_I4.h"

#include "cmp_ge_I8.h"
#include "cmp_le_I8.h"
#include "cmp_eq_I8.h"
#include "cmp_ne_I8.h"
#include "cmp_gt_I8.h"
#include "cmp_lt_I8.h"

#include "cmp_ge_F4.h"
#include "cmp_le_F4.h"
#include "cmp_eq_F4.h"
#include "cmp_ne_F4.h"
#include "cmp_gt_F4.h"
#include "cmp_lt_F4.h"

#include "nn_core.h"

#include "concat_I4_I4_to_I8.h"




//---------------------------------------------------------------
// START FUNC DECL 
int 
vec_f1f2opf3(
	     long long in_lb,
	     long long in_ub,
	     FLD_TYPE f1type,
	     FLD_TYPE f2type,
	     char *f1_X,
	     char *nn_f1_X,
	     char *f2_X,
	     char *nn_f2_X,
	     char *op,
	     FLD_TYPE f3type,
	     char *op_X,
	     char *nn_op_X
	     )
// STOP FUNC DECL 
{
  int status = 0;
  bool is_nn_field_needed = false;
  int nT = INT_MIN; long long block_size;
  
  if ( f1type != f2type ) { 
    fprintf(stderr, "f1 and f2 are not same type \n"); go_BYE(-1);
  }
  //--------------------------------------------
  status = partition((in_ub-in_lb), 8192, -1, &block_size, &nT); cBYE(status);
  //--------------------------------------------
  if ( ( nn_f1_X == NULL ) && ( nn_f2_X == NULL ) ) {
    is_nn_field_needed = false;
  }
  else {
    is_nn_field_needed = true;
    for ( int tid = 0; tid < nT; tid++ ) {
      long long lb = in_lb + (tid * block_size);
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = in_ub; }
      long long nX = (ub -lb);
      if ( ( nn_f1_X == NULL ) && ( nn_f2_X != NULL ) ) {
	memcpy(nn_op_X+lb, nn_f2_X+lb, nX);
      }
      else if ( ( nn_f1_X != NULL ) && ( nn_f2_X == NULL ) ) {
	memcpy(nn_op_X+lb, nn_f1_X+lb, nX);
      }
      else {
	nn_core(nn_f1_X+lb, nn_f2_X+lb, nX, nn_op_X+lb);
      }
    }
  }
  // Now for the real computation
  //-------------------------------------------------
  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = in_lb + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = in_ub; }
    long long nX = (ub -lb);
    unsigned long long *g1UI8 = NULL, *g2UI8 = NULL, *opUI8 = NULL;
    long long          *g1I8 = NULL,  *g2I8 = NULL,  *opI8 = NULL;
    float              *g1F4 = NULL,  *g2F4 = NULL,  *opF4 = NULL; 
    double             *g1F8 = NULL,  *g2F8 = NULL,  *opF8 = NULL; 
    char      *opI1 = NULL;
    char      *g1_X = NULL, *g2_X = NULL, *my_op_X = NULL;
    switch ( f1type ) {
      case I4 : 
        g1_X = (f1_X + ( sizeof(int) * lb ));
        g2_X = (f2_X + ( sizeof(int) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  my_op_X = (op_X + ( sizeof(int) * lb ));
	  add2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  sub2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  mul2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  div2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "%") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  rem2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "concat") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  concat_I4_I4_to_I8((unsigned int *)g1_X, (unsigned int *)g2_X, nX, (unsigned long long *)my_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_I4((int *)g1_X, (int *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_and2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_or2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_xor2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "<<") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  shiftleft2_I4((int *)g1_X, (int *)g2_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, ">>") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  shiftright2_UI4((unsigned int *)g1_X, (unsigned int *)g2_X, nX, (unsigned int *)my_op_X);
	}
	else { 
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
	break;
      case I8 : 
	g1UI8 = (unsigned long long *)f1_X; g1UI8 += lb;
	g2UI8 = (unsigned long long *)f2_X; g2UI8 += lb;
	g1I8  = (long long          *)f1_X; g1I8  += lb;
	g2I8  = (long long          *)f2_X; g2I8  += lb;
	opI8  = (long long          *)op_X; opI8  += lb; 
	opUI8 = (unsigned long long *)op_X; opUI8 += lb; 
	opI1  = (char               *)op_X; opI1  += lb; 
	if ( strcmp(op, "+") == 0 ) {
	  add2_I8(g1I8, g2I8, nX, opI8); 
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub2_I8(g1I8, g2I8, nX, opI8); 
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul2_I8(g1I8, g2I8, nX, opI8); 
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div2_I8(g1I8, g2I8, nX, opI8); 
	}
	else if ( strcmp(op, "%") == 0 ) {
	  rem2_I8(g1I8, g2I8, nX, opI8); 
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, "<=") == 0 ) {
	  cmp_le_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, "==") == 0 ) {
	  cmp_eq_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, "!=") == 0 ) {
	  cmp_ne_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, ">") == 0 ) {
	  cmp_gt_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, "<") == 0 ) {
	  cmp_lt_I8(g1I8, g2I8, nX, opI1);
	}
	else if ( strcmp(op, "&") == 0 ) {
	  bwise_and2_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "a_and_not_b") == 0 ) {
	  bwise_a_and_not_b_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "|") == 0 ) {
	  bwise_or2_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "^") == 0 ) {
	  bwise_xor2_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "<<") == 0 ) {
	  shiftleft2_I8(g1I8, g2I8, nX, opI8);
	}
	else if ( strcmp(op, ">>") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  shiftright2_UI8(g1UI8, g2UI8, nX, opUI8);
	}
	else { 
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
      break;
      case F4 : 
        g1F4 = (float *)f1_X; g1F4 += lb;
        g2F4 = (float *)f2_X; g2F4 += lb;
	opI1 = (char  *)op_X; opI1 += lb;
	opF4 = (float *)op_X; opF4 += lb;
	if ( strcmp(op, "+") == 0 ) {
	  add2_F4(g1F4, g2F4, nX, opF4);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub2_F4(g1F4, g2F4, nX, opF4);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul2_F4(g1F4, g2F4, nX, opF4);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div2_F4(g1F4, g2F4, nX, opF4);
	}
	else if ( strcmp(op, ">=") == 0 ) {
	  cmp_ge_F4(g1F4, g2F4, nX, opI1);
	}
	else if ( strcmp(op, "<=") == 0 ) {
	  cmp_le_F4(g1F4, g2F4, nX, opI1);
	}
	else if ( strcmp(op, "==") == 0 ) {
	  cmp_eq_F4(g1F4, g2F4, nX, opI1);
	}
	else if ( strcmp(op, "!=") == 0 ) {
	  cmp_ne_F4(g1F4, g2F4, nX, opI1);
	}
	else if ( strcmp(op, ">") == 0 ) {
	  cmp_gt_F4(g1F4, g2F4, nX, opI1);
	}
	else if ( strcmp(op, "<") == 0 ) {
	  cmp_lt_F4(g1F4, g2F4, nX, opI1);
	}
	else { 
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
	break;
      case F8 : 
        g1F8 = (double *)f1_X; g1F8 += lb;
        g2F8 = (double *)f2_X; g2F8 += lb;
	opI1 = (char   *)op_X; opI1 += lb;
	opF8 = (double *)op_X; opF8 += lb;
	if ( strcmp(op, "+") == 0 ) {
	  add2_F8(g1F8, g2F8, nX, opF8);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub2_F8(g1F8, g2F8, nX, opF8);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul2_F8(g1F8, g2F8, nX, opF8);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div2_F8(g1F8, g2F8, nX, opF8);
	}
	/* TODO P2
	else if ( strcmp(op, ">=") == 0 ) {
	  cmp_ge_F8(g1F8, g2F8, nX, opI1);
	}
	else if ( strcmp(op, "<=") == 0 ) {
	  cmp_le_F8(g1F8, g2F8, nX, opI1);
	}
	else if ( strcmp(op, "==") == 0 ) {
	  cmp_eq_F8(g1F8, g2F8, nX, opI1);
	}
	else if ( strcmp(op, "!=") == 0 ) {
	  cmp_ne_F8(g1F8, g2F8, nX, opI1);
	}
	else if ( strcmp(op, ">") == 0 ) {
	  cmp_gt_F8(g1F8, g2F8, nX, opI1);
	}
	else if ( strcmp(op, "<") == 0 ) {
	  cmp_lt_F8(g1F8, g2F8, nX, opI1);
	}
	*/
	else { 
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
	break;
      case I1 : 
        g1_X = (f1_X + ( sizeof(char) * lb ));
        g2_X = (f2_X + ( sizeof(char) * lb ));
        my_op_X = (op_X + ( sizeof(char) * lb ));
	if ( strcmp(op, "||") == 0 ) {
	  or2_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&&") == 0 ) {
	  and2_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&&") == 0 ) {
	  and2_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&&!") == 0 ) {
	  a_and_not_b_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
	  cmp_ne_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
	  cmp_eq_I1((char *)g1_X, (char *)g2_X, nX, (char *)my_op_X);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
	break;
      case B : 
        g1UI8 = (unsigned long long *)f1_X; 
        g2UI8 = (unsigned long long *)f2_X; 
        opUI8 = (unsigned long long *)op_X; 
	if ( ( ( lb / NUM_BITS_IN_I8 ) * NUM_BITS_IN_I8 ) != lb ) {
	  WHEREAMI; status = -1; continue;
	}
	lb /= NUM_BITS_IN_I8;
	nX /= NUM_BITS_IN_I8; /* TODO: P0 What when not multiple of 64 */
	g1UI8 += lb;
	g2UI8 += lb;
	opUI8 += lb;
	if ( strcmp(op, "||") == 0 ) {
	  bwise_or2_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "&&") == 0 ) {
	  bwise_and2_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else if ( strcmp(op, "&&!") == 0 ) {
	  bwise_a_and_not_b_I8(g1UI8, g2UI8, nX, opUI8);
	}
	else {
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	}
	break;
      default :
	  if ( status == 0 ) { WHEREAMI; } status = -1; continue;
	break;
    }
  }
  cBYE(status);
  //--------------------------------------------
 BYE:
  return(status);
}
