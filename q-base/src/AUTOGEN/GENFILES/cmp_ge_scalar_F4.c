#include "macros.h"
// START FUNC DECL
void
cmp_ge_scalar_F4( 
			    float *in1, 
			    float in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
#pragma simd
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ >= in2;
  }
}
