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
#include "constants.h"

// START FUNC DECL
void uniq_I8(
    long long *in,
    long long n_in,
    char *nn,
    long long *out,
    long long *ptr_n_out,
    int *cnt
    )
// STOP FUNC DECL
{ 
  bool is_good_val_seen = false;
  long long last_good_val = 0; // initialization to suppress compiler warning
  long long n_out = 0, cnt_ptr = 0;

  for ( long long i = 0; i < n_in ; i++ ) { 
    if ( ( nn != NULL ) && ( nn[i] == FALSE ) ) {
      continue;
    }
    if ( is_good_val_seen == false ) {
      is_good_val_seen = true;
      last_good_val = out[n_out] = in[i];
      cnt[cnt_ptr] = 1;
      n_out++;
    }
    else {
      if ( in[i] == last_good_val ) {
	cnt[cnt_ptr] = cnt[cnt_ptr] + 1;
      }
      else {
        last_good_val = out[n_out] = in[i];
	n_out++;
	cnt_ptr++;
        cnt[cnt_ptr] = 1;
      }
    }
  }
  *ptr_n_out = n_out;
}
