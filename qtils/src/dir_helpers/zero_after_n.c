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
zero_after_n(
    char *X,
    long long nR,
    long long one_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  long long *Y = NULL; long long nY, i, cnt = 0;

  if ( X == NULL ) { go_BYE(-1); }
  nY = nR / NUM_BITS_IN_I8;
  if ( ( nY / NUM_BITS_IN_I8 ) != nR )  { nY++; }
  Y = (long long *)X;

  for ( i = 0; i < nY; i++ ) {
    cnt +=  __builtin_popcountll(Y[i]);
    if ( cnt >= one_cnt ) { 
      i++;
      break; 
    }
  }
  /* set rest to 0 TODO P2: Use IPP version */
  for ( ; i < nY; i++ ) {
    Y[i] = 0; 
  }
BYE:
  return(status);
}


