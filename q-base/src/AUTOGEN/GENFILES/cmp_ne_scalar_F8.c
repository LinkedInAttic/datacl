#include "macros.h"
// START FUNC DECL
void
cmp_ne_scalar_F8( 
			    double *in1, 
			    double in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
#pragma simd
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ != in2;
  }
}
