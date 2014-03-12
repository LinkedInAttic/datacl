#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include <stdlib.h>
#include "permute_I4.h"

// START FUNC DECL
int
permute_I4(
    int *X,
    long long n
    )
// STOP FUNC DECL
{
  int status = 0;
  struct drand48_data buffer;
  srand48_r(852187451, &buffer);
  if ( X == NULL ) { go_BYE(-1); }
  if ( n <= 1 ) { go_BYE(-1); }
  for (long long k = n-1; k > 0; k--) {
    long long pos; 
    double dtemp;
    int swap; 
    drand48_r(&buffer, &dtemp);
    pos = dtemp * n; if ( pos == n ) { pos--; }
    swap   = X[pos];
    X[pos] = X[k];
    X[k]   = swap;
  }
BYE:
  return(status);
}
