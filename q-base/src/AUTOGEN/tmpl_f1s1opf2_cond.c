#include "macros.h"
// START FUNC DECL
void
cmp___OP___scalar___INTYPE__( 
			    __INTYPE2__ *in1, 
			    __INTYPE2__ in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
#pragma simd
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ __OP2__ in2;
  }
}
