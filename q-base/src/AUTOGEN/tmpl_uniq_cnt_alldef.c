#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "constants.h"

// START FUNC DECL
void uniq_cnt_alldef___XTYPE__(
    __ITYPE__ *in,
    int *in_cnt,
    long long n_in,
    __ITYPE__ *out,
    long long *ptr_n_out,
    int *out_cnt
    )
// STOP FUNC DECL
{ 
  __ITYPE__ last_val_seen; /* This is the value whose number of
			      occurrences we are trying to count */
  __ITYPE__ first_val; /* This is the very first value */
  __ITYPE__ inval; 
  long long n_out;
  long long counter; // Keeps cumulative of incnt for same value of inval
  int incnt;

  // Consume first value 
  n_out = 0;
  first_val = in[0]; 
  last_val_seen = first_val;
  counter = 1; 
  for ( long long i = 1; i < n_in ; i++ ) { 
    inval = in[i];
    incnt = in_cnt[i];
    if ( inval == last_val_seen ) {
      counter += incnt;
    }
    else {
      out[n_out] = last_val_seen;
      out_cnt[n_out] = counter;
      n_out++; // increment number of unique values seen
      last_val_seen = inval;
      counter = incnt; // reset counter
    }
  }
  /* Take care of last value */
  out[n_out] = last_val_seen;
  out_cnt[n_out] = counter;
  n_out++;
  *ptr_n_out = n_out;
}
