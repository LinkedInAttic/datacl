#include <math.h>
// START FUNC DECL
void
vec_abs_F8( 
			    double *in, 
			    int nR, 
			    double *out
			    )
// STOP FUNC DECL
{
  int i;
  for ( i = 0; i < nR; i++ ) { 
    *out = (double)fabs(*in);
    in++;
    out++;
  }
}

