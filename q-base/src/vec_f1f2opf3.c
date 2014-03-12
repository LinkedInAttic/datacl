#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#include "ipps.h"
#endif
#ifdef ICC
#include <cilk/cilk.h>
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "vec_f1f2opf3.h"
#include "get_type_op_fld.h"
#include "mk_file.h"
#include "assign_I1.h"

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

#include "add2_I2.h"
#include "sub2_I2.h"
#include "mul2_I2.h"
#include "div2_I2.h"
#include "rem2_I2.h"

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

#include "cmp_ge_I2.h"
#include "cmp_le_I2.h"
#include "cmp_eq_I2.h"
#include "cmp_ne_I2.h"
#include "cmp_gt_I2.h"
#include "cmp_lt_I2.h"

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

#include "cmp_ge_F8.h"
#include "cmp_le_F8.h"
#include "cmp_eq_F8.h"
#include "cmp_ne_F8.h"
#include "cmp_gt_F8.h"
#include "cmp_lt_F8.h"


#include "concat_I4_I4_to_I8.h"


// last review 9/6/2013


//---------------------------------------------------------------
// START FUNC DECL 
int 
vec_f1f2opf3(
	     long long nR,
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
  int nT = INT_MIN; long long block_size;
  
  if ( f1type != f2type ) { 
    fprintf(stderr, "f1 and f2 are not same type \n"); go_BYE(-1);
  }
  //--------------------------------------------
  status = partition(nR, 4096, -1, &block_size, &nT); cBYE(status);
  //--------------------------------------------
  if ( ( nn_f1_X != NULL ) || ( nn_f2_X != NULL ) ) {
#pragma omp parallel for 
    for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
      long long lb = (tid * block_size);
      long long ub = lb + block_size;
      if ( tid == (nT-1) ) { ub = nR; }
      long long nX = (ub -lb);
      if ( ( nn_f1_X != NULL ) && ( nn_f2_X != NULL ) ) {
	and2_I1(nn_f1_X+lb, nn_f2_X+lb, nX, nn_op_X+lb);
      }
      else if ( nn_f1_X != NULL ) { 
	assign_I1(nn_op_X+lb, nn_f1_X+lb, nX);
      }
      else if ( nn_f2_X != NULL ) { 
	assign_I1(nn_op_X+lb, nn_f2_X+lb, nX);
      }
      else {
	status = -1; continue;
      }
    }
    cBYE(status);
  }
  // Now for the real computation
  //-------------------------------------------------
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    long long lb = tid * block_size;
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = (ub -lb);
    long long b_lb, b_nX; // used for f1type == B 

    char               *g1I1 = NULL, *g2I1 = NULL, *opI1 = NULL;
    short              *g1I2 = NULL, *g2I2 = NULL, *opI2 = NULL;
    int                *g1I4 = NULL, *g2I4 = NULL, *opI4 = NULL;
    long long          *g1I8 = NULL, *g2I8 = NULL, *opI8 = NULL; 
    float              *g1F4 = NULL, *g2F4 = NULL, *opF4 = NULL; 
    double             *g1F8 = NULL, *g2F8 = NULL, *opF8 = NULL; 

    unsigned char      *g1UI1 = NULL,*g2UI1 = NULL,*opUI1 = NULL; 
    unsigned int       *g1UI4 = NULL,*g2UI4 = NULL,*opUI4 = NULL; 
    unsigned long long *g1UI8 = NULL,*g2UI8 = NULL,*opUI8 = NULL; 

    if ( ( nn_f1_X == NULL ) && ( nn_f2_X == NULL ) ) {
      /* Nothing to do */
    }
    else if ( ( nn_f1_X == NULL ) && ( nn_f2_X != NULL ) ) {
      memcpy(nn_op_X+lb, nn_f2_X+lb, nX);
    }
    else if ( ( nn_f1_X != NULL ) && ( nn_f2_X == NULL ) ) {
      memcpy(nn_op_X+lb, nn_f1_X+lb, nX);
    }
    else if ( ( nn_f1_X != NULL ) && ( nn_f2_X != NULL ) ) {
      and2_I1(nn_f1_X+lb, nn_f2_X+lb, nX, nn_op_X+lb);
    }
    else {
      status = -1; continue;
    }

    switch ( f1type ) {
    case I4 : 
      g1I4 = (int  *)f1_X; g1I4 += lb;
      g2I4 = (int  *)f2_X; g2I4 += lb;
      opI4 = (int  *)op_X; opI4 += lb;
      opI1 = (char *)op_X; opI1 += lb;

      g1UI4 = (unsigned int  *)f1_X; g1UI4 += lb;
      g2UI4 = (unsigned int  *)f2_X; g2UI4 += lb;
      opUI4 = (unsigned int  *)op_X; opUI4 += lb;
      opUI8 = (unsigned long long  *)op_X; opUI8 += lb;
      if ( strcmp(op, "+") == 0 ) {
	add2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, "/") == 0 ) {
	div2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, "concat") == 0 ) {
	concat_I4_I4_to_I8(g1UI4, g2UI4, nX, opUI8);
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_I4(g1I4, g2I4, nX, opI1);
      }
      else if ( strcmp(op, "&") == 0 ) {
	bwise_and2_I4(g1UI4, g2UI4, nX, opUI4);
      }
      else if ( strcmp(op, "|") == 0 ) {
	bwise_or2_I4(g1UI4, g2UI4, nX, opUI4);
      }
      else if ( strcmp(op, "^") == 0 ) {
	bwise_xor2_I4(g1UI4, g2UI4, nX, opUI4);
      }
      else if ( strcmp(op, "<<") == 0 ) {
	shiftleft2_I4(g1I4, g2I4, nX, opI4);
      }
      else if ( strcmp(op, ">>") == 0 ) {
	shiftright2_UI4(g1UI4, g2UI4, nX, opUI4);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case I8 : 
      g1I8 = (long long *)f1_X; g1I8 += lb;
      g2I8 = (long long *)f2_X; g2I8 += lb;
      opI8 = (long long *)op_X; opI8 += lb; 
      opI1 = (char      *)op_X; opI1 += lb; 

      g1UI8 = (unsigned long long *)f1_X; g1UI8 += lb;
      g2UI8 = (unsigned long long *)f2_X; g2UI8 += lb;
      opUI8 = (unsigned long long *)op_X; opUI8 += lb; 
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
	shiftright2_UI8(g1UI8, g2UI8, nX, opUI8);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case I2 : 
      g1I2 = (short *)f1_X; g1I2 += lb;
      g2I2 = (short *)f2_X; g2I2 += lb;
      opI2 = (short *)op_X; opI2 += lb; 
      opI1 = (char  *)op_X; opI1 += lb; 
      if ( strcmp(op, "+") == 0 ) {
	add2_I2(g1I2, g2I2, nX, opI2); 
      }
      else if ( strcmp(op, "-") == 0 ) {
	sub2_I2(g1I2, g2I2, nX, opI2); 
      }
      else if ( strcmp(op, "*") == 0 ) {
	mul2_I2(g1I2, g2I2, nX, opI2); 
      }
      else if ( strcmp(op, "/") == 0 ) {
	div2_I2(g1I2, g2I2, nX, opI2); 
      }
      else if ( strcmp(op, "%") == 0 ) {
	rem2_I2(g1I2, g2I2, nX, opI2); 
      }
      else if ( strcmp(op, ">=") == 0 ) {
	cmp_ge_I2(g1I2, g2I2, nX, opI1);
      }
      else if ( strcmp(op, "<=") == 0 ) {
	cmp_le_I2(g1I2, g2I2, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_I2(g1I2, g2I2, nX, opI1);
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_I2(g1I2, g2I2, nX, opI1);
      }
      else if ( strcmp(op, ">") == 0 ) {
	cmp_gt_I2(g1I2, g2I2, nX, opI1);
      }
      else if ( strcmp(op, "<") == 0 ) {
	cmp_lt_I2(g1I2, g2I2, nX, opI1);
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
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case I1 : 
      g1I1 = f1_X; g1I1 += lb;
      g2I1 = f2_X; g2I1 += lb;
      opI1 = op_X; opI1 += lb;

      g1UI1 = (unsigned char *)f1_X; g1UI1 += lb;
      g2UI1 = (unsigned char *)f2_X; g2UI1 += lb;
      opUI1 = (unsigned char *)op_X; opUI1 += lb;

      if ( strcmp(op, "||") == 0 ) {
	or2_I1(g1I1, g2I1, nX, opI1);
      }
      else if ( strcmp(op, "&&") == 0 ) {
	and2_I1(g1I1, g2I1, nX, opI1);
      }
      else if ( strcmp(op, "&&") == 0 ) {
	and2_I1(g1I1, g2I1, nX, opI1);
      }
      else if ( strcmp(op, "&&!") == 0 ) {
	a_and_not_b_I1(g1UI1, g2UI1, nX, opUI1); 
      }
      else if ( strcmp(op, "!=") == 0 ) {
	cmp_ne_I1(g1I1, g2I1, nX, opI1);
      }
      else if ( strcmp(op, "==") == 0 ) {
	cmp_eq_I1(g1I1, g2I1, nX, opI1);
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
      b_lb = lb / NUM_BITS_IN_I8;
      b_nX = nX / NUM_BITS_IN_I8; 
      /* TODO: P0 What when not multiple of 64 */
      g1UI8 += b_lb;
      g2UI8 += b_lb;
      opUI8 += b_lb;
      if ( strcmp(op, "||") == 0 ) {
	bwise_or2_I8(g1UI8, g2UI8, b_nX, opUI8);
      }
      else if ( strcmp(op, "&&") == 0 ) {
	bwise_and2_I8(g1UI8, g2UI8, b_nX, opUI8);
      }
      else if ( strcmp(op, "&&!") == 0 ) {
	bwise_a_and_not_b_I8(g1UI8, g2UI8, b_nX, opUI8);
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
