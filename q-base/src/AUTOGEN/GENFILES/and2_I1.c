#include <stdio.h>
typedef long long longlong;
// START FUNC DECL
void
and2_I1( 
			    char *in1, 
			    char *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  char inv1, inv2, outv;
  for ( long long i = 0; i < nR; i++ ) { 
    inv1 = *in1;
    inv2 = *in2;
    outv = inv1 && inv2;
    *out = outv;
    in1++;
    in2++;
    out++;
  }
}
