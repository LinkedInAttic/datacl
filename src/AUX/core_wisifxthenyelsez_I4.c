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
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"
// START FUNC DECL
void
core_wisifxthenyelsez_I4(
				  char *X,
				  char *nn_X,
				  long long nR,
				  int yval,
				  char *in_Y,
				  char *nn_Y,
				  int zval,
				  char *in_Z,
				  char *nn_Z,
				  char *in_W,
				  char *nn_W,
				  bool *ptr_is_some_null
				  )
// STOP FUNC DECL
{
  int *W = (int *)in_W;
  int *Y = (int *)in_Y;
  int *Z = (int *)in_Z;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( nn_X == NULL ) || ( nn_X[i] == 1 ) ) { 
      if ( X[i] == 1 ) { /* x is true => w := y */
	if ( in_Y == NULL ) {  /*use scalar value */
	  W[i] = yval;
	  nn_W[i] = 1;
	}
	else {
	  if ( ( nn_Y == NULL ) || ( nn_Y[i] == 1 ) ) {
	    W[i] = Y[i];
	    nn_W[i] = 1;
	  }
	  else {
	    W[i] = 0;
	    nn_W[i] = 0;
	    *ptr_is_some_null = false;
	  }
	}
      }
      else if ( X[i] == 0 ) {
	if ( in_Z == NULL ) {  /*use scalar value */
	  W[i] = zval;
	  nn_W[i] = 1;
	}
	else {
	  if ( ( nn_Z == NULL ) || ( nn_Z[i] == 1 ) ) {
	    W[i] = Z[i];
	    nn_W[i] = 1;
	  }
	  else {
	    W[i] = 0;
	    nn_W[i] = 0;
	    *ptr_is_some_null = false;
	  }
	}
      }
      else {
#ifdef CILK
	fprintf(stderr, "Error: Control should never come here\n");
#else
	exit(-1);
#endif
      }
    }
    else { 
      W[i] = 0;
      nn_W[i] = 0;
      *ptr_is_some_null = false;
    }
  }
}
