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
pos_mmad_I8_I4_nn(
    long long *f1, 
    char *nn_f1,
    long long nR1, 
    int *f2, 
    char *nn_f2,
    long long nR2, 
    long long *ptr_minval,
    long long *ptr_maxval,
    long long *ptr_sum,
    long long *ptr_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  long long minval = *ptr_minval;
  long long maxval = *ptr_maxval;
  long long sum = *ptr_sum; 
  long long cnt = *ptr_cnt; 
  for ( long long i = 0; i < nR2; i++ ) {
    if ( ( nn_f2 != NULL ) && ( nn_f2[i] == FALSE ) ) { continue; }
    int f1idx = f2[i];
    if ( ( nn_f1 != NULL ) && ( nn_f1[f1idx] == FALSE ) ) { continue; }
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
    long long f1val = f1[f1idx];
    if ( f1val < minval ) { minval = f1val; }
    if ( f1val > maxval ) { maxval = f1val; }
    sum += (long long)f1val;
    cnt++;
  }
  *ptr_minval = minval;
  *ptr_maxval = maxval;
  *ptr_sum    = sum;
  *ptr_cnt    = cnt;
BYE:
  return(status);
}

