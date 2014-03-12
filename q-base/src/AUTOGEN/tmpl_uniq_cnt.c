#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"

// START FUNC DECL
void uniq_cnt___XTYPE__(
    __ITYPE__ *in,
    int *in_cnt,
    long long n_in,
    char *nn,
    __ITYPE__ *out,
    long long *ptr_n_out,
    int *cnt
    )
// STOP FUNC DECL
{ 
  bool is_good_val_seen = false;
  __ITYPE__ last_good_val = 0; // initialization to suppress compiler warning
  long long n_out = 0, cnt_ptr = 0;
  int incnt;

  for ( long long i = 0; i < n_in ; i++ ) { 
    if ( ( nn != NULL ) && ( nn[i] == FALSE ) ) {
      continue;
    }
    if ( is_good_val_seen == false ) {
      is_good_val_seen = true;
      last_good_val = out[n_out] = in[i];
      incnt = in_cnt[i];
      cnt[cnt_ptr] = incnt;
      n_out++;
    }
    else {
      if ( in[i] == last_good_val ) {
        incnt = in_cnt[i];
	cnt[cnt_ptr] += incnt;
      }
      else {
        last_good_val = out[n_out] = in[i];
	n_out++;
	cnt_ptr++;
        incnt = in_cnt[i];
        cnt[cnt_ptr] = incnt;
      }
    }
  }
  *ptr_n_out = n_out;
}
