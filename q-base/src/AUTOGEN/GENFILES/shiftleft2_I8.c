#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
shiftleft2_I8( 
			    long long *in1, 
			    long long *in2, 
			    long long nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  long long inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 << inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
