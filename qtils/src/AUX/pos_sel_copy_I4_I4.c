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
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_sel_copy_I4_I4(
    int *f1, 
    long long nR1, 
    int *f2, 
    long long nR2, 
    int lb,
    int ub,
    int *f3,
    long long *ptr_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  long long cnt = 0;
  long long i3 = 0;
  for ( long long i2 = 0; i2 < nR2; i2++ ) {
    int f1idx = f2[i2];
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
    int val = f1[f1idx];
    if ( ( val >= lb ) && ( val <= ub ) ) {
      f3[i3++] = val;
      cnt++;
    }
  }
  *ptr_cnt = cnt;
BYE:
  return(status);
}

