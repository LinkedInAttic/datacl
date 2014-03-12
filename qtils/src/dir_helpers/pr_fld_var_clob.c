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
pr_fld_var_clob(
    char *X,
    long long lb,
    long long ub,
    const char *nn_X,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  char *cptr = NULL;
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      // skip over this entry 
      cptr = X;
      for ( ; *cptr != '\0' ; ) { cptr++; } 
      cptr++; // skip over nullc
      X = cptr;
      continue;
    }
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) {
      X++; // skip over nullc
      fprintf(ofp, "\"\"\n");
      continue;
    }
    fprintf(ofp, "\"");
    for ( char *cptr = X;  *cptr != '\0'; ) {
      fprintf(ofp, "%c", *cptr++);
    }
    fprintf(ofp, "\"");
    fprintf(ofp, "\n");
    cptr++; // skip over nullc;
    X = cptr;
  }
  return(status);
}
