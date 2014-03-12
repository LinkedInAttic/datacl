#include <stdio.h>
// START FUNC DECL
void add_scalar_int(
			    int *in1, 
			    int in2, 
			    int nR, 
			    int *out
			    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nR; i++ ) { 
    *out++ = *in1++ + in2;
  }
}
