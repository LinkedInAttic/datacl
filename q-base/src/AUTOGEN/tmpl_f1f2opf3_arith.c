#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
__OP__2___TYPE__( 
			    __TYPE2__ *in1, 
			    __TYPE2__ *in2, 
			    long long nR, 
			    __TYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  __TYPE2__ inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 __OP2__ inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
