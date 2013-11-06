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
#include <stdbool.h>
// START FUNC DECL
void
cmp_eq_mult_scalar_simple_I2( 
			      short *in1, 
			      long long n1, 
			      short *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    short inval = in1[i];
    bool found = false;
    for ( int j = 0; j < n2; j++ ) {
      if ( inval == in2[j] ) {
        found = true; 
        break;
      }
    }
    *out++ = found;
  }
}
