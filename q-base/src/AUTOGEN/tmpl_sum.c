#include <stdio.h>
#include "macros.h"
// START FUNC DECL
void
sum___INTYPE__( 
			    __INTYPE2__ *in, 
			    char *nn,
			    long long nR, 
			    __INTYPE3__ *ptr_numer,
			    long long *ptr_denom
			    )
// STOP FUNC DECL
{
  __INTYPE3__ numer = 0;
  long long denom = 0;
  if ( nn == NULL ) { 
    denom += nR;
    for ( long long i = 0; i < nR; i++ ) { 
      numer += *in++;
    }
  }
  else {
    for ( long long i = 0; i < nR; i++ ) { 
      if ( *nn == 1 ) { 
        numer += *in;
	denom += 1;
      }
      nn++;
      in++;
    }
  }
  *ptr_numer += numer;
  *ptr_denom += denom;
}

