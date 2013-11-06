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
which_bin_I4(
		    int inval,
		    int *lb_vals,
		    int *ub_vals,
		    int nvals,
		    int *ptr_idx,
		    bool *ptr_exists
		    )
// STOP FUNC DECL
{
  int idx = -1;
  bool exists = false;
  for ( int i = 0; i < nvals; i++ ) { 
    if ( ( inval >= lb_vals[i] ) &&  ( inval <= ub_vals[i] ) ) {
      idx = i;
      exists = true;
      break;
    }
  }
  *ptr_idx = idx;
  *ptr_exists = exists;
}
