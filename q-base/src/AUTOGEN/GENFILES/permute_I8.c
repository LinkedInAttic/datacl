#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include <stdlib.h>

// START FUNC DECL
int
permute_I8(
    long long *X,
    long long n
    )
// STOP FUNC DECL
{
  int status = 0;
#ifdef MAC_OSX
	// TODO P0 to complete
#else
  struct drand48_data buffer;
  srand48_r(852187451, &buffer);
#endif
  if ( X == NULL ) { go_BYE(-1); }
  if ( n <= 1 ) { go_BYE(-1); }
  for (long long k = n-1; k >= 0; k--) {
    long long pos; 
    double dtemp;
    long long swap; 
#ifdef MAC_OSX
		dtemp = drand48();
#else
    drand48_r(&buffer, &dtemp);
#endif
    pos = dtemp * n; if ( pos == n ) { pos--; }
    swap   = X[pos];
    X[pos] = X[k];
    X[k]   = swap;
  }
BYE:
  return(status);
}
