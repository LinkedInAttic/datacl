#include <math.h>
// START FUNC DECL
void
vec_reciprocal_F4( 
			    float *in, 
			    int nR, 
			    float *out
			    )
// STOP FUNC DECL
{
  int i;
  register float one = 1.0;
  for ( i = 0; i < nR; i++ ) { 
    *out = one / *in;
    in++;
    out++;
  }
}

