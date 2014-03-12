#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "extract_I1.h"
#include "extract_I2.h"
#include "extract_I4.h"
#include "extract_I8.h"
#include "s_to_f_const_I1.h"
#include "s_to_f_const_I2.h"
#include "s_to_f_const_I4.h"
#include "s_to_f_const_I8.h"
#include "s_to_f_const_F4.h"
#include "s_to_f_seq_I1.h"
#include "s_to_f_seq_I2.h"
#include "s_to_f_seq_I4.h"
#include "s_to_f_seq_I8.h"
#include "vec_s_to_f.h"
#include "extract_S.h"

// last review 9/4/2013
// START FUNC DECL
int
vec_s_to_f(
	   char *X,
	   long long nR,
	   const char *str_scalar
	   )
// STOP FUNC DECL
{
  int status = 0;
  int       valI4, startI4, incrI4;
  long long valI8, startI8, incrI8;
  char      valI1, startI1, incrI1;
  short     valI2, startI2, incrI2;
  float     valF4; 
#define MAX_LEN 32
  char op[MAX_LEN];
  char fldtype[MAX_LEN];
  bool is_null = true;
  int nT; long long block_size;

  // Get information about what to do 
  zero_string(op, MAX_LEN);
  zero_string(fldtype, MAX_LEN);
  status = extract_S(str_scalar, "op=[", "]", op, MAX_LEN, &is_null); 
  cBYE(status); if ( is_null ) { go_BYE(-1); }

  status = extract_S(str_scalar, "fldtype=[", "]", fldtype, MAX_LEN, &is_null); 
  cBYE(status); if ( is_null ) { go_BYE(-1); }
  status = partition(nR, 4096, -1, &block_size, &nT); cBYE(status);
  //----------------------------------------
  if ( strcmp(op, "const") == 0 ) {
    if ( strcmp(fldtype, "I1") == 0 ) {
      status = extract_I1(str_scalar, "val=[", "]", &valI1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(str_scalar, "val=[", "]", &valI2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(str_scalar, "val=[", "]", &valI4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "F4") == 0 ) {
      status = extract_F4(str_scalar, "val=[", "]", &valF4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(str_scalar, "val=[", "]", &valI8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else {
      go_BYE(-1);
    }
  }
  else if ( strcmp(op, "seq") == 0 ) { 
    if ( strcmp(fldtype, "I1") == 0 ) {
      status = extract_I1(str_scalar, "start=[", "]", &startI1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I1(str_scalar, "incr=[", "]",  &incrI1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      if ( incrI1 == 0 ) { go_BYE(-1); }
      startI8 = startI1; incrI8 = incrI1;
      if ( startI8 > SCHAR_MAX ) { go_BYE(-1); }
      if ( startI8 < SCHAR_MIN ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) > SCHAR_MAX ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) < SCHAR_MIN ) {go_BYE(-1); }
    }
    else if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(str_scalar, "start=[", "]", &startI2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I2(str_scalar, "incr=[", "]",  &incrI2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      if ( incrI2 == 0 ) { go_BYE(-1); }
      startI8 = startI2; incrI8 = incrI2;
      if ( startI8 > SHRT_MAX ) { go_BYE(-1); }
      if ( startI8 < SHRT_MIN ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) > SHRT_MAX ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) < SHRT_MIN ) {go_BYE(-1); }
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(str_scalar, "start=[", "]", &startI4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I4(str_scalar, "incr=[", "]",  &incrI4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      if ( incrI4 == 0 ) { go_BYE(-1); }
      startI8 = startI4; incrI8 = incrI4;
      if ( startI8 > INT_MAX ) { go_BYE(-1); }
      if ( startI8 < INT_MIN ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) > INT_MAX ) { go_BYE(-1); }
      if ( startI8 + (nR * incrI8) < INT_MIN ) {go_BYE(-1); }
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(str_scalar, "start=[", "]", &startI8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I8(str_scalar, "incr=[", "]",  &incrI8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      if ( incrI8 == 0 ) { go_BYE(-1); }
    }
    else {
      go_BYE(-1);
    }
  }
  else {
    go_BYE(-1);
  }
  //----------------------------------------

#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    char      *XI1 = NULL; char      lstartI1;
    short     *XI2 = NULL; short     lstartI2;
    int       *XI4 = NULL; int       lstartI4;
    long long *XI8 = NULL; long long lstartI8;
    float     *XF4 = NULL; 

    long long lb = tid * block_size;
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = (ub -lb);

    if ( strcmp(op, "const") == 0 ) {
      if ( strcmp(fldtype, "I1") == 0 ) {
	XI1 = (char *)X; XI1 += lb;
#ifdef IPP
	ippsSet_8u(valI1, XI1, nX);
#else
	s_to_f_const_I1(XI1, nX, valI1); 
#endif
      }
      else if ( strcmp(fldtype, "I2") == 0 ) {
	XI2 = (short *)X; XI2 += lb;
#ifdef IPP
	ippsSet_16s(valI2, XI2, nX);
#else
	s_to_f_const_I2(XI2, nX, valI2); 
#endif
      }
      else if ( strcmp(fldtype, "I4") == 0 ) {
	XI4 = (int *)X; XI4 += lb;
#ifdef IPP
	ippsSet_32s(valI4, XI4, nX);
#else
	s_to_f_const_I4(XI4, nX, valI4); 
#endif
      }
      else if ( strcmp(fldtype, "F4") == 0 ) {
	XF4 = (float *)X; XF4 += lb;
#ifdef IPP
	ippsSet_32f(valF4, XF4, nX);
#else

	s_to_f_const_F4(XF4, nX, valF4); 
#endif
      }
      else if ( strcmp(fldtype, "I8") == 0 ) {
	XI8 = (long long *) X; XI8 += lb;
#ifdef IPP
	ippsSet_64s(valI8, XI8, nX);
#else
	s_to_f_const_I8(XI8, nX, valI8);
#endif
      }
    }
    else if ( strcmp(op, "seq") == 0 ) {
      if ( strcmp(fldtype, "I1") == 0 ) {
	XI1 = (char *)X; XI1 += lb;
        lstartI1 = startI1 + (lb * incrI1);
	s_to_f_seq_I1(XI1, nX, lstartI1, incrI1); 
      }
      else if ( strcmp(fldtype, "I2") == 0 ) {
	XI2 = (short *)X; XI2 += lb;
        lstartI2 = startI2 + (lb * incrI2);
	s_to_f_seq_I2(XI2, nX, lstartI2, incrI2); 
      }
      else if ( strcmp(fldtype, "I4") == 0 ) {
	XI4 = (int *)X; XI4 += lb;
        lstartI4 = startI4 + (lb * incrI4);
	s_to_f_seq_I4(XI4, nX, lstartI4, incrI4); 
      }
      else if ( strcmp(fldtype, "I8") == 0 ) {
	XI8 = (long long *) X; XI8 += lb;
        lstartI8 = startI8 + (lb * incrI8);
	s_to_f_seq_I8(XI8, nX, lstartI8, incrI8); 
      }
    }
    else {
      status = -1; continue;
    }
  }
  cBYE(status);
 BYE:
  return status ;
}
