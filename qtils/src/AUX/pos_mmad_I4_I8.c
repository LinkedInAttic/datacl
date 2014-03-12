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
#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_mmad_I4_I8(
    int *f1, 
    long long nR1, 
    long long *f2, 
    long long nR2, 
    int *ptr_minval,
    int *ptr_maxval,
    long long *ptr_sum
    )
// STOP FUNC DECL
{
  int status = 0;
  int minval = *ptr_minval;
  int maxval = *ptr_maxval;
  long long sum = *ptr_sum;
  for ( long long i = 0; i < nR2; i++ ) {
    long long f1idx = f2[i];
#ifdef DEBUG
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
#endif
    int f1val = f1[f1idx];
    if ( f1val < minval ) { minval = f1val; }
    if ( f1val > maxval ) { maxval = f1val; }
    sum += (long long)f1val;
  }
  cBYE(status);
  *ptr_minval = minval;
  *ptr_maxval = maxval;
  *ptr_sum    = sum;
BYE:
  return(status);
}
