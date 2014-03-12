#include <stdio.h>
// START FUNC DECL
void
__MINMAXOP_____INTYPE__( 
			    __INTYPE2__ *in, 
			    char *nn,
			    int nR, 
			    __INTYPE2__ *ptr_rslt
			    )
// STOP FUNC DECL
{
  if ( nn == NULL ) { 
  for ( long long i = 0; i < nR; i++ ) { 
    if ( *in __MINMAXOP2__ *ptr_rslt ) {
      *ptr_rslt = *in;
    }
    in++;
  }
  }
  else {
  for ( long long i = 0; i < nR; i++ ) { 
    if ( *nn == 1 ) { 
    if ( *in __MINMAXOP2__ *ptr_rslt ) {
      *ptr_rslt = *in;
    }
    }
    nn++;
    in++;
  }
  }
}

