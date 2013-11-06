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
#include "par_s_to_f.h"
#include "extract_S.h"

// START FUNC DECL
int
par_s_to_f(
	   char *X,
	   long long in_lb,
	   long long in_ub,
	   const char *str_scalar
	   )
// STOP FUNC DECL
{
  int status = 0;
  int val4 = INT_MAX, start4, incr4;
  long long val8 = LLONG_MAX, start8, incr8;
  float valF4 = FLT_MAX; 
  char val1, start1, incr1;
  short val2, start2, incr2;
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
  status = partition((in_ub - in_lb), 1024, -1, &block_size, &nT); cBYE(status);
  //----------------------------------------
  if ( strcmp(op, "const") == 0 ) {
    if ( strcmp(fldtype, "I1") == 0 ) {
      status = extract_I1(str_scalar, "val=[", "]", &val1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(str_scalar, "val=[", "]", &val2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(str_scalar, "val=[", "]", &val4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "F4") == 0 ) {
      status = extract_F4(str_scalar, "val=[", "]", &valF4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(str_scalar, "val=[", "]", &val8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else {
      go_BYE(-1);
    }
  }
  else if ( strcmp(op, "seq") == 0 ) { 
    if ( strcmp(fldtype, "I1") == 0 ) {
      status = extract_I1(str_scalar, "start=[", "]", &start1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I1(str_scalar, "incr=[", "]",  &incr1, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(str_scalar, "start=[", "]", &start2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I2(str_scalar, "incr=[", "]",  &incr2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(str_scalar, "start=[", "]", &start4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I4(str_scalar, "incr=[", "]",  &incr4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(str_scalar, "start=[", "]", &start8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I8(str_scalar, "incr=[", "]",  &incr8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else {
      go_BYE(-1);
    }
  }
  else {
    go_BYE(-1);
  }
  //----------------------------------------

  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = in_lb + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = in_ub; }
    long long nX = (ub -lb);
    if ( strcmp(op, "const") == 0 ) {
      if ( strcmp(fldtype, "I1") == 0 ) {
	char *XI1 = (char *)X; XI1 += lb;
	s_to_f_const_I1(XI1, nX, val1); 
      }
      else if ( strcmp(fldtype, "I2") == 0 ) {
	short *XI2 = (short *)X; XI2 += lb;
	s_to_f_const_I2(XI2, nX, val2); 
      }
      else if ( strcmp(fldtype, "I4") == 0 ) {
	int *XI4 = (int *)X; XI4 += lb;
	s_to_f_const_I4(XI4, nX, val4); 
      }
      else if ( strcmp(fldtype, "F4") == 0 ) {
	float *XF4 = (float *)X; XF4 += lb;
	s_to_f_const_F4(XF4, nX, valF4); 
      }
      else if ( strcmp(fldtype, "I8") == 0 ) {
	long long *XI8 = (long long *) X; XI8 += lb;
	s_to_f_const_I8(XI8, nX, val8);
      }
    }
    else if ( strcmp(op, "seq") == 0 ) {
      if ( strcmp(fldtype, "I1") == 0 ) {
        long long l_start = start1 + (lb * incr1);
	if ( l_start > SCHAR_MAX ) { status = -1; continue; }
	s_to_f_seq_I1(((char *)X)+lb, nX, (char)l_start, incr1); 
      }
      else if ( strcmp(fldtype, "I2") == 0 ) {
        long long l_start = start2 + (lb * incr2);
	if ( l_start > SHRT_MAX ) { status = -1; continue; }
	s_to_f_seq_I2(((short *)X)+lb, nX, (short)l_start, incr2); 
      }
      else if ( strcmp(fldtype, "I4") == 0 ) {
        long long l_start = start4 + (lb * incr4);
	if ( l_start > INT_MAX ) { status = -1; continue; }
	s_to_f_seq_I4(((int *)X)+lb, nX, (int)l_start, incr4); 
      }
      else if ( strcmp(fldtype, "I8") == 0 ) {
        long long l_start = start8 + (lb * incr8);
	s_to_f_seq_I8(((long long *)X)+lb, nX, (long long)l_start, incr8); 
      }
    }
    else {
      status = -1; continue;
    }
  }
  cBYE(status);
 BYE:
  return(status);
}
