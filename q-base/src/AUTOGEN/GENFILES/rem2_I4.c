#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
rem2_I4( 
			    int *in1, 
			    int *in2, 
			    long long nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  int inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 % inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
