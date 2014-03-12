#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
bwise_and2_I4( 
			    unsigned int *in1, 
			    unsigned int *in2, 
			    long long nR, 
			    unsigned int * restrict out
			    )
// STOP FUNC DECL
{
  unsigned int inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 & inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
