#include <stdio.h>
#include <limits.h>
#include "macros.h"
// START FUNC DECL
int
find_smallest_geq_I4(
    int *X, /* [n] */
    long long n, 
    int val,
    long long *ptr_idx
    )
// STOP FUNC DECL
{
  int status = 0;
  long long mid, prev_mid = LLONG_MIN;
  long long bot = 0;
  long long top = n-1;
  if ( X == NULL ) { go_BYE(-1); }
  if ( n <= 1 ) { go_BYE(-1); }

  for ( ; ; ) {
    mid = ( bot + top ) / 2;
    if ( mid == prev_mid ) { *ptr_idx = bot; break; }
    if ( X[mid] > val ) { /* we need to look in bottom half */
      top = mid;
    }
    else if ( X[mid] < val ) { /* we need to look in bottom half */
      bot = mid;
    }
    else {
      *ptr_idx = mid; break; 
    }
    prev_mid = mid;
  }
BYE:
  return(status);
}

#undef UNIT_TEST
#ifdef UNIT_TEST
int
main()
{
  int status = 0;
#define N 33
  int X[N];
  long long idx;

  for ( int   i = 0; i < N; i++ ) { 
    X[i] = 2*i;
  }
  int val = 10000;
  status = find_smallest_geq_I4(X, (long long)N, val, &idx); 
  cBYE(status);
  fprintf(stderr, "idx = %lld \n", idx);
BYE:
  return(status);
}


#endif
