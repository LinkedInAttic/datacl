#include <stdio.h>
#define BITS_PER_BYTE 8
// START FUNC DECL
void
bitcount___XTYPE__( 
		    unsigned __ITYPE__ *in, 
		    long long nR, 
		    char * restrict out
		    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    out[i] = __builtin_popcount__XX__(in[i]);
  }
}
