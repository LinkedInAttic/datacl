#include "spooky_hash.h"
// START FUNC DECL
void
hash_I8( 
			    unsigned long long *in, 
			    long long nR, 
			    unsigned long long seed,
			    unsigned long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = spooky_hash64(in, sizeof(long long), seed);
    in++;
    out++;
  }
}

