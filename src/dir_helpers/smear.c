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
smear(
    char *in,
    long long nR,
    int plus,
    int minus,
    char *out
    )
// STOP FUNC DECL
{
  int status = 0;

  if ( minus < 0 ) { go_BYE(-1); }
  if ( plus  < 0 ) { go_BYE(-1); }
  for ( long long i = 0; i < nR; i++ ) { 
    if ( in[i] == 1 ) { 
      long long lb = i - minus; if ( lb < 0 ) { lb = 0; } 
      long long ub = i + 1 + plus;  if ( ub >= nR ) { ub = nR; } 
      for ( long long j = lb; j < ub; j++ ) { 
	out[j] = 1;
      }
    }
  }
BYE:
  return(status);
}


