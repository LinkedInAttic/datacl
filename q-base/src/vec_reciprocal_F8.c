#include <math.h>
// START FUNC DECL
void
vec_reciprocal_F8( 
			    double *in, 
			    int nR, 
			    double *out
			    )
// STOP FUNC DECL
{
  int i;
  register double one = 1.0;
  for ( i = 0; i < nR; i++ ) { 
    *out = one / *in;
    in++;
    out++;
  }
}

