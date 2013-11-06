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
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_count_bitvec_I8_I4(
    long long *f1, 
    long long nR1, 
    int *f2, 
    long long nR2, 
    int *f3,
    long long nR3
    )
// STOP FUNC DECL
{
  int status = 0;
  for ( long long i = 0; i < nR2; i++ ) {
    int f1idx = f2[i];
#ifdef DEBUG
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
#endif
    unsigned long long bitvec = f1[f1idx];
    for ( int i = 0; i < nR3; i++ ) { 
      if ( bitvec == 0 ) { break; }
      if ( ( bitvec & 1 ) == 1 ) {
	f3[i]++; // TODO P2 speed this up with ptr arith int *tempf3 = f3;
      }
      bitvec = bitvec >> 1; 
    }
  }
BYE:
  return(status);
}

