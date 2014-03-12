#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
sub2_F4( 
			    float *in1, 
			    float *in2, 
			    long long nR, 
			    float * restrict out
			    )
// STOP FUNC DECL
{
  float inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 - inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
