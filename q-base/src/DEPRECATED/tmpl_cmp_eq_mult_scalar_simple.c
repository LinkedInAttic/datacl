#include <stdbool.h>
// START FUNC DECL
void
cmp_eq_mult_scalar_simple___INTYPE__( 
			      __INTYPE2__ *in1, 
			      long long n1, 
			      __INTYPE2__ *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    __INTYPE2__ inval = in1[i];
    // While this loop might look strange, it vectorizes nicely
    register int n_match = 0;
    for ( int j = 0; j < n2; j++ ) {
      n_match += ( inval == in2[j] );
    }
    if ( n_match == 0 ) { *out++ = 0; } else { *out++ = 1; }
  }
}
