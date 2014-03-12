#include <stdio.h>
// START FUNC DECL
void add_scalar_longlong(
			    long long *in1, 
			    long long in2, 
			    int nR, 
			    long long *out
			    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nR; i++ ) { 
    *out++ = *in1++ + in2;
  }
}
