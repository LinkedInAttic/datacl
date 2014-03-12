#ifdef IPP
#include "ipps.h"
#include "ippvm.h"
#endif
#include <stdio.h>
#include "qtypes.h"

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

#include "conv_F4_to_I4.h"
#include "conv_F4_to_I8.h"
#include "conv_F4_to_F8.h"

#include "nn_conv_F8_to_I4.h"
#include "nn_conv_F8_to_I8.h"
#include "nn_conv_F8_to_F4.h"

#include "nn_conv_I4_to_I1.h"
#include "nn_conv_I4_to_I2.h"
#include "nn_conv_I4_to_I8.h"
#include "nn_conv_I4_to_F4.h"
#include "nn_conv_I4_to_F8.h"

#include "nn_conv_I8_to_I4.h"
#include "nn_conv_I8_to_I1.h"
#include "nn_conv_I8_to_F4.h"
#include "nn_conv_I8_to_F8.h"

#include "nn_conv_I1_to_I4.h"
#include "nn_conv_I1_to_I8.h"

#include "nn_conv_F4_to_I4.h"
#include "nn_conv_F4_to_I8.h"
#include "nn_conv_F4_to_F8.h"

// START FUNC DECL
int
conv(
     char *in,
     long long lb,
     long long ub,
     char *nn,
     FLD_TYPE in_fldtype,
     char * restrict out,
     FLD_TYPE out_fldtype
     )
// STOP FUNC DECL
{
  int status = 0;
  char      *f1I1 = NULL, *opI1 = NULL;
  short                   *opI2 = NULL;
  int       *f1I4 = NULL, *opI4 = NULL;
  long long *f1I8 = NULL, *opI8 = NULL;
  float     *f1F4 = NULL, *opF4 = NULL;
  double    *f1F8 = NULL, *opF8 = NULL;
  long long nX = ub - lb;

  if ( nn == NULL ) {
    switch ( in_fldtype ) {
    case F8 :
      f1F8 = (double *)in; f1F8 += lb;
      switch ( out_fldtype ) {
      case F4 : 
        opF4 = (float *)out; opF4 += lb;
#ifdef IPP
	ippsConvert_64f32f(f1F8, opF4, nX);
#else
	conv_F8_to_F4(f1F8, nX, opF4);
#endif
	break;
      case I8 : 
        opI8 = (long long *)out; opI8 += lb;
	conv_F8_to_I8(f1F8, nX, opI8); 
	break;
      case I4 : 
        opI4 = (int *)out; opI4 += lb;
	conv_F8_to_I4(f1F8, nX, opI4); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case I4 : 
      f1I4 = (int *)in; f1I4 += lb;
      switch ( out_fldtype ) {
      case I1 : 
	opI1 = (char *)out; opI1 += lb;
	conv_I4_to_I1(f1I4, nX, opI1); 
	break;
      case I2 : 
	opI2 = (short *)out; opI2 += lb;
	conv_I4_to_I2(f1I4, nX, opI2); 
	break;
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	conv_I4_to_I8(f1I4, nX, opI8); 
	break;
      case F4 : 
	opF4 = (float *)out; opF4 += lb;
	conv_I4_to_F4(f1I4, nX, opF4); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	conv_I4_to_F8(f1I4, nX, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case I8: 
      f1I8 = (long long *)in; f1I8 += lb;
      switch ( out_fldtype ) {
      case I1 : 
	opI1 = (char *)out; opI1 += lb;
	conv_I8_to_I1(f1I8, nX, opI1); 
	break;
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	conv_I8_to_I4(f1I8, nX, opI4); 
	break;
      case F4 : 
	opF4 = (float *)out; opF4 += lb;
	conv_I8_to_F4(f1I8, nX, opF4); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	conv_I8_to_F8(f1I8, nX, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case F4 : 
      f1F4 = (float *)in; f1F4 += lb;
      switch ( out_fldtype ) {
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	conv_F4_to_I4(f1F4, nX, opI4); 
	break;
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	conv_F4_to_I8(f1F4, nX, opI8); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	conv_F4_to_F8(f1F4, nX, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case B : 
      // TODO : P0 Need to offset pointers 
      switch ( out_fldtype ) { 
      case I1 : 
	if ( ( ( lb / NUM_BITS_IN_I1 ) * NUM_BITS_IN_I1 ) != lb ) {
	  WHEREAMI; status = -1; break; 
	}
	f1I1 += (lb / NUM_BITS_IN_I1);
	status = conv_B_to_I1(f1I1, nX, opI1);
	if ( status < 0 ) { WHEREAMI; break; }
	break;
      default : status= -1; break; 
      }
      break;
    case I1 : 
      f1I1 = (char *)in; f1I1 += lb;
      switch ( out_fldtype ) {
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	conv_I1_to_I8(f1I1, nX, opI8); 
	break;
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	conv_I1_to_I4(f1I1, nX, opI4); 
	break;
      case B : 
	if ( lb % NUM_BITS_IN_I1 != 0 ) { WHEREAMI; status = -1; break; }
        opI1 = (char *)out; opI1 += (lb / NUM_BITS_IN_I1);
	status = conv_I1_to_B(f1I1, nX, opI1);
	if ( status < 0  ) { WHEREAMI; status = -1; break; }
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    default : 
      status= -1; 
      break;
    }
  }
  else {
    nn += lb;
    switch ( in_fldtype ) {
    case F8 :
      f1F8 = (double *)in; f1F8 += lb;
      switch ( out_fldtype ) {
      case F4 : 
        opF4 = (float *)out; opF4 += lb;
	nn_conv_F8_to_F4(f1F8, nX, nn, opF4);
	break;
      case I8 : 
        opI8 = (long long *)out; opI8 += lb;
	nn_conv_F8_to_I8(f1F8, nX, nn, opI8); 
	break;
      case I4 : 
        opI4 = (int *)out; opI4 += lb;
	nn_conv_F8_to_I4(f1F8, nX, nn, opI4); 
	break;
      default : 
	status= -1; 
	break;
      }
    case I4 : 
      f1I4 = (int *)in; f1I4 += lb;
      switch ( out_fldtype ) { 
      case I1 : 
	opI1 = (char *)out; opI1 += lb;
	nn_conv_I4_to_I1(f1I4, nX, nn, opI1); 
	break;
      case I2 : 
	opI2 = (short *)out; opI2 += lb;
	nn_conv_I4_to_I2(f1I4, nX, nn, opI2); 
	break;
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	nn_conv_I4_to_I8(f1I4, nX, nn, opI8); 
	break;
      case F4 : 
	opF4 = (float *)out; opF4 += lb;
	nn_conv_I4_to_F4(f1I4, nX, nn, opF4); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	nn_conv_I4_to_F8(f1I4, nX, nn, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case I8: 
      f1I8 = (long long *)in; f1I8 += lb;
      switch ( out_fldtype ) {
      case I1 : 
	opI1 = (char *)out; opI1 += lb;
	nn_conv_I8_to_I1(f1I8, nX, nn, opI1); 
	break;
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	nn_conv_I8_to_I4(f1I8, nX, nn, opI4); 
	break;
      case F4 : 
	opF4 = (float *)out; opF4 += lb;
	nn_conv_I8_to_F4(f1I8, nX, nn, opF4); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	nn_conv_I8_to_F8(f1I8, nX, nn, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case F4 : 
      f1F4 = (float *)in; f1F4 += lb;
      switch ( out_fldtype ) {
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	nn_conv_F4_to_I4(f1F4, nX, nn, opI4); 
	break;
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	nn_conv_F4_to_I8(f1F4, nX, nn, opI8); 
	break;
      case F8 : 
	opF8 = (double *)out; opF8 += lb;
	nn_conv_F4_to_F8(f1F4, nX, nn, opF8); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    case I1 : 
      f1I1 = (char *)in; f1I1 += lb;
      switch ( out_fldtype ) {
      case I8 : 
	opI8 = (long long *)out; opI8 += lb;
	nn_conv_I1_to_I8(f1I1, nX, nn, opI8); 
	break;
      case I4 : 
	opI4 = (int *)out; opI4 += lb;
	nn_conv_I1_to_I4(f1I1, nX, nn, opI4); 
	break;
      default : 
	status= -1; 
	break;
      }
      break;
    default : 
      status= -1; 
      break;
    }
  }
  return status ;
}
