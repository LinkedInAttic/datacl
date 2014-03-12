#include "spooky_hash.h"
// START FUNC DECL
void
hash_I4( 
			    unsigned int *in, 
			    long long nR, 
			    unsigned long long seed,
			    unsigned long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = spooky_hash64(in, sizeof(int), seed);
    in++;
    out++;
  }
}

