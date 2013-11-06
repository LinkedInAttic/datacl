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
#include "constants.h"
#include "macros.h"
#include <stdio.h>
// START FUNC DECL
int
xfer_F8_I4(
				  char *src_fld_X, 
				  char *nn_src_fld_X, 
				  long long src_nR,
				  char *dst_idx_X, 
				  char *nn_dst_idx_X, 
				  long long dst_nR,
				  char *dst_fld_X, 
				  char *nn_dst_fld_X
				  )
// STOP FUNC DECL
{
  int status = 0;

  for ( long long i = 0; i < dst_nR; i++ ) { 
  double  val;
  int   idx;

  int  *idxptr = (int  *) dst_idx_X;
  double *valptr = (double *)src_fld_X;
  double *outptr = (double *)dst_fld_X;

    if ( ( nn_dst_idx_X != NULL ) && ( nn_dst_idx_X[i] == 0 ) ) {
      nn_dst_fld_X[i] = 0;
      outptr[i]       = 0;
    }
    else {
      idx = idxptr[i];
      if ( ( idx < 0 ) || ( idx >= src_nR ) ) { go_BYE(-1); }
      val = valptr[idx];
      if ( ( nn_src_fld_X != NULL ) && ( nn_src_fld_X[idx] == 0 ) ) {
        nn_dst_fld_X[i] = 0;
        outptr[i]       = 0;
      }
      else {
	if ( nn_dst_fld_X != NULL ) { 
          nn_dst_fld_X[i] = 1;
	}
        outptr[i]       = val;
      }
    }
  }
 BYE:
  return(status);
}

