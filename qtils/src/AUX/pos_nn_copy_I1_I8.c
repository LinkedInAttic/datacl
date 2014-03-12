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
pos_nn_copy_I1_I8(
    char *f1, 
    char *nn_f1,
    long long nR1, 
    long long *f2, 
    char *nn_f2,
    long long nR2, 
    char *f3,
    long long *ptr_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  long long cnt = 0;
  for ( long long i = 0; i < nR2; i++ ) {
    if ( ( nn_f2 != NULL ) && ( nn_f2[i] == FALSE ) )  { continue; }
    long long f1idx = f2[i];
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
    if ( ( nn_f1 != NULL ) && ( nn_f1[f1idx] == FALSE ) )  { continue; }
    char ival = f1[f1idx];
    f3[cnt++] = ival;
  }
  *ptr_cnt = cnt;
BYE:
  return(status);
}
