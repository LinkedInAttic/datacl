#ifdef IPP
#include "ipps.h"
#include "ippvm.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "vec_f1opf2.h"
#include "get_type_op_fld.h"
#include "extract_S.h"
#include "extract_I8.h"
#include "mk_temp_file.h"
#include "assign_I1.h"
#include "conv.h"

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


#include "hash_I4.h"
#include "hash_I8.h"
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

// last review 9/5/2013
//---------------------------------------------------------------
// START FUNC DECL 
int 
vec_f1opf2(
	   long long nR,
	   FLD_TYPE src_fldtype,
	   char *f1_X,
	   char *nn_f1_X,
	   char *op,
	   char *f2_X,
	   char *nn_f2_X,
	   FLD_TYPE dst_fldtype
	   )
// STOP FUNC DECL 
{
  int status = 0;
#define BUFLEN 32
  unsigned long long seedUI8 = 0;
  int nT; long long block_size;
  /*---------------------------------------------*/
  status = partition(nR, 1024, -1, &block_size, &nT); cBYE(status);
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    if ( status < 0 ) { continue; }

    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = (ub -lb);

    char *nn = nn_f1_X; nn +=  lb;
    char      *f1I1 = NULL, *opI1 = NULL;
    short     *f1I2 = NULL, *opI2 = NULL;
    int       *f1I4 = NULL, *opI4 = NULL;
    long long *f1I8 = NULL, *opI8 = NULL;
    float     *f1F4 = NULL, *opF4 = NULL;
    double    *f1F8 = NULL, *opF8 = NULL;
    unsigned long long *opUI8 = NULL;
    unsigned int       *f1UI4 = NULL;
    unsigned long long *f1UI8 = NULL;

    f1I1  = (char               *)f1_X; f1I1  += lb; 
    f1I2  = (short              *)f1_X; f1I2  += lb; 
    f1I4  = (int                *)f1_X; f1I4  += lb; 
    f1I8  = (long long          *)f1_X; f1I8  += lb; 
    f1F4  = (float              *)f1_X; f1F4  += lb; 
    f1F8  = (double             *)f1_X; f1F8  += lb; 
    f1UI8 = (unsigned long long *)f1_X; opUI8 += lb;
    f1UI4 = (unsigned int       *)f1_X; f1UI4 += lb;

    opI1  = (char               *)f2_X; opI1  += lb;
    opI2  = (short              *)f2_X; opI2  += lb;
    opI4  = (int                *)f2_X; opI4  += lb;
    opI8  = (long long          *)f2_X; opI8  += lb;
    opF4  = (float              *)f2_X; opF4  += lb;
    opF8  = (double             *)f2_X; opF8  += lb;
    opUI8 = (unsigned long long *)f2_X; opUI8 += lb;

    /* START: Handle the nn field */
    if ( nn_f1_X != NULL ) {
#ifdef IPP
      ippsCopy_8u(nn_f1_X+lb, nn_f2_X+lb, nX);
#else
      assign_I1(nn_f2_X+lb, nn_f1_X+lb, nX); 
#endif
    }
    /* STOP: Handle the nn field */
    switch ( src_fldtype ) {
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
      }
      else if ( strcmp(op, "sqrt") == 0 ) {
#ifdef IPP
	ippsSqrt_64f(f1F8, opF8, nX);
#else
	vec_sqrt_F8(f1F8,  nX, opF8);
#endif
      }
      else if ( strcmp(op, "abs") == 0 ) {
#ifdef IPP
	ippsAbs_64f_A53(f1F8, opF8, nX);
#else
	vec_abs_F8(f1F8, nX, opF8);
#endif
      }
      else if ( strcmp(op, "reciprocal") == 0 ) {
	vec_reciprocal_F8(f1F8, nX, opF8);
      }
      else if ( strcmp(op, "normal_cdf_inverse") == 0 ) {
	vec_normal_cdf_inverse(f1F8, nX, nn_f2_X, opF8);
      }
      else if ( strcmp(op, "pval_from_zval") == 0 ) {
	vec_pval_from_zval(f1F8, nX, opF8);
      }
      else {
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case I4 : 
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
      }
      else if ( strcmp(op, "hash") == 0 ) {
	hash_I4(f1UI4, nX, seedUI8, opUI8);
      }
      else if ( strcmp(op, "!") == 0 ) {
	not_I4(f1I4, nX, opI4);
      }
      else if ( strcmp(op, "~") == 0 ) {
	ones_complement_I4(f1I4, nX, opI4);
      }
      else if ( strcmp(op, "++") == 0 ) {
	incr_I4(f1I4, nX, opI4);
      }
      else if ( strcmp(op, "--") == 0 ) {
	decr_I4(f1I4, nX, opI4);
      }
      else if ( strcmp(op, "bitcount") == 0 ) {
	bitcount_I4(f1UI4, nX, opI1);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case I8: 
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
	if ( status < 0 ) { WHEREAMI; }
      }
      else if ( strcmp(op, "hash") == 0 ) {
	hash_I8(f1UI8, nX, seedUI8, opUI8);
      }
      else if ( strcmp(op, "!") == 0 ) {
	not_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "~") == 0 ) {
	ones_complement_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "++") == 0 ) {
	incr_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "--") == 0 ) {
	decr_I8(f1I8, nX, opI8);
      }
      else if ( strcmp(op, "bitcount") == 0 ) {
	bitcount_I8(f1UI8, nX, opI1);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case F4 : 
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
      }
      else if ( strcmp(op, "sqrt") == 0 ) {
#ifdef IPP
	ippsSqrt_32f(f1F4, opF4, nX);
#else
	vec_sqrt_F4(f1F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, "abs") == 0 ) {
#ifdef IPP
	ippsAbs_32f_A24(f1F4, opF4, nX);
#else
	vec_abs_F4(f1F4, nX, opF4);
#endif
      }
      else if ( strcmp(op, "reciprocal") == 0 ) {
	vec_reciprocal_F4(f1F4, nX, opF4);
      }
      else { 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue;
      }
      break;
    case B : 
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
      }
      else {
	if ( status == 0 ) { 
	  fprintf(stderr, "Invalid op = [%s] \n", op); WHEREAMI; 
	}
	status = -1; continue;
      }
      break;
    case I1 : 
      if ( strcmp(op, "conv") == 0 ) {
	status = conv(f1_X, lb, ub, nn_f1_X, src_fldtype, f2_X, dst_fldtype);
      }
      else if ( strcmp(op, "!") == 0 ) {
	not_I1(f1I1, nX, opI1);
      }
      else if ( strcmp(op, "~") == 0 ) {
	ones_complement_I1(f1I1, nX, opI1);
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
 BYE:
  return(status);
}
