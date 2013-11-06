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
#include "../src/constants.h"
#include "../src/macros.h"
// START FUNC DECL
int
countf_nn_I4(
		int *fk_fld,
		int *src_fld,
		long long n,
		char *cfld,
		long long * restrict cnt,
		int n_out
		)
// STOP FUNC DECL
{
  int status = 0;
  register char  *cptr = cfld;
  for ( long long i = 0; i < n; i++ ) {
    if ( *cptr++ == FALSE ) { continue; }
    int idx = fk_fld[i];
    int val = src_fld[i];
    int idxI4 = (int)idx;
#undef DEBUG
#ifdef DEBUG
    if ( ( idx < 0 ) || ( idx >= n_out ) ) { go_BYE(-1); }
#endif
    cnt[idxI4] += val;
  }
#ifdef DEBUG
 BYE:
#endif
  return(status);
}
