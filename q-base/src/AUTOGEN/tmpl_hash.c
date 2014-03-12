#include "spooky_hash.h"
// START FUNC DECL
void
hash___XTYPE__( 
			    unsigned __ITYPE__ *in, 
			    long long nR, 
			    unsigned long long seed,
			    unsigned long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = spooky_hash64(in, sizeof(__ITYPE__), seed);
    in++;
    out++;
  }
}

