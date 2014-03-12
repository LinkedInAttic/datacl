#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
div2_F8( 
			    double *in1, 
			    double *in2, 
			    long long nR, 
			    double * restrict out
			    )
// STOP FUNC DECL
{
  double inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 / inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
