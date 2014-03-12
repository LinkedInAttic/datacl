#include <math.h>
// START FUNC DECL
void
vec_abs_F4( 
			    float *in, 
			    int nR, 
			    float *out
			    )
// STOP FUNC DECL
{
  int i;
  for ( i = 0; i < nR; i++ ) { 
    *out = (float)fabsf(*in);
    in++;
    out++;
  }
}

