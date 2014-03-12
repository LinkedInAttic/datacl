#include "macros.h"
// START FUNC DECL
void
cmp_lt_scalar_I1( 
			    char *in1, 
			    char in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
#pragma simd
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ < in2;
  }
}
