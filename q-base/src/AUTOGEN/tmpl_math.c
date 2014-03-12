#include <math.h>
// START FUNC DECL
void
vec___LIBFN__( 
			    __INTYPE__ *in, 
			    long long nR, 
			    __INTYPE__ * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (__INTYPE__)__LIBFN__(*in);
    in++;
    out++;
  }
}

