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
typedef long long longlong;
// START FUNC DECL
void
rem2_I4( 
			    int *in1, 
			    int *in2, 
			    long long nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  int inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 % inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
