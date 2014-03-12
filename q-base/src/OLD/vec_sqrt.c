#include <math.h>
// START FUNC DECL
void
vec_sqrt( 
			    float *in, 
			    int nR, 
			    float *out
			    )
// STOP FUNC DECL
{
  int i;
  for ( i = 0; i < nR; i++ ) { 
    *out = (float)sqrt(*in);
    in++;
    out++;
  }
}

