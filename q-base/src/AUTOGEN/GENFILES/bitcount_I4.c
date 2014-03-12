#include <stdio.h>
#define BITS_PER_BYTE 8
// START FUNC DECL
void
bitcount_I4( 
		    unsigned int *in, 
		    long long nR, 
		    char * restrict out
		    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    out[i] = __builtin_popcount(in[i]);
  }
}
