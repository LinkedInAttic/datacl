#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
mul2_I2( 
			    short *in1, 
			    short *in2, 
			    long long nR, 
			    short * restrict out
			    )
// STOP FUNC DECL
{
  short inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 * inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
