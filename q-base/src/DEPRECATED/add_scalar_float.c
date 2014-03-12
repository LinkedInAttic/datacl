#include <stdio.h>
// START FUNC DECL
void add_scalar_float(
			    float *in1, 
			    float in2, 
			    int nR, 
			    float *out
			    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nR; i++ ) { 
    *out++ = *in1++ + in2;
  }
}
