#include <stdio.h>
#include <stdbool.h>
#include "../src/constants.h"
#include "../src/macros.h"
// START FUNC DECL
int
count_I2(
		short *in,
		long n,
		long long * restrict cnt,
		int n_out,
		bool safe_mode
		)
// STOP FUNC DECL
{
  int status = 0;
  if ( safe_mode == true ) {
    for ( long long i = 0; i < n; i++ ) {
      short idx = in[i];
      int idxI4 = (int )idx;
      if ( ( idx < 0 ) || ( idx >= n_out ) ) { status = -1; continue; }
      cnt[idxI4]++;
    }
  }
  else {
    for ( long long i = 0; i < n; i++ ) {
      short idx = in[i];
      int idxI4 = (int )idx;
      cnt[idxI4]++;
    }
  }
  return status ;
}
