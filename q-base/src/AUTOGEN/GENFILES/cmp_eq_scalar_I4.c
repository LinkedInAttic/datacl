#include "macros.h"
// START FUNC DECL
void
cmp_eq_scalar_I4( 
			    int *in1, 
			    int in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
#pragma simd
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ == in2;
  }
}
