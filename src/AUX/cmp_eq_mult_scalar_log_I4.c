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
cmp_eq_mult_scalar_log_I4( 
			      int *in1, 
			      long long n1, 
			      int *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    bool found = false;
    int inval = in1[i];
    int ub = n2;
    int lb = 0;
    int probe;
    int val_to_cmp;
    bool is_last_iter = false;
    for ( ;  ub > lb;  ) {
      probe = (lb + ub) / 2 ;
      if ( ( probe == lb ) || ( probe == ub ) )  { 
	is_last_iter = true;
      }
      val_to_cmp = in2[probe];
      if ( inval == val_to_cmp ) { 
	found = true;
	break;
      }
      else if ( inval > val_to_cmp ) {
	lb = probe;
      }
      else if ( inval < val_to_cmp ) { 
	ub = probe;
      }
      else { /* control cannot come here */ }
      if ( is_last_iter ) { break; }
    }
    *out++ = found;
  }
}
