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
#include <unistd.h>
#include "constants.h"

// START FUNC DECL
void uniq_cnt_alldef_ll_dbl(
    long long *in,
    double *in_cnt,
    long long n_in,
    long long *out,
    long long *ptr_n_out,
    int *out_cnt
    )
// STOP FUNC DECL
{ 
  long long last_val_seen; /* This is the value whose number of
			      occurrences we are trying to count */
  long long first_val; /* This is the very first value */
  long long inval; 
  long long n_out;
  long long counter; // Counts the number of similar values we have seen

  // Consume first value 
  n_out = 0;
  first_val = in[0]; 
  last_val_seen = first_val;
  counter = 1; 
  for ( long long i = 1; i < n_in ; i++ ) { 
    inval = in[i];
    if ( inval == last_val_seen ) {
      counter++;

    }
    else {
      out[n_out] = last_val_seen;
      out_cnt[n_out] = counter;
      n_out++; // increment number of unique values seen
      last_val_seen = inval;
      counter = 1; // reset counter
    }
  }
  /* Take care of last value */
  out[n_out] = last_val_seen;
  out_cnt[n_out] = counter;
  n_out++;
  *ptr_n_out = n_out;
}
