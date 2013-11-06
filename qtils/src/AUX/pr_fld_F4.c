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
// START FUNC DECL
int
pr_fld_F4(
    float *X,
    long long lb,
    long long ub,
    const char *nn_X,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  float val;
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      continue;
    }
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) {
      fprintf(ofp, "\"\"\n");
      continue;
    }
    val = X[i];
    if ( nn_X == NULL ) { 
      fprintf(ofp, "%f\n", val);
    }
    else {
      fprintf(ofp, "\"%f\"\n", val);
    }
  }
  return(status);
}
