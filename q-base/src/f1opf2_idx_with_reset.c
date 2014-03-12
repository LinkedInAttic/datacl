#include "qtypes.h"
#include "f1opf2_idx_with_reset.h"

// START FUNC DECL
int 
f1opf2_idx_with_reset(
	   const char *in,
	   long long nR,
	   long long * restrict out
	   )
// STOP FUNC DECL
{
  int status = 0;
  long long idx = 0;

  if ( nR <= 0 ) { go_BYE(-1); }
  if ( in == NULL ) { go_BYE(-1); }
  if ( out== NULL ) { go_BYE(-1); }
  for ( long long i = 0; i < nR; i++ ) {
    if (in[i] == 1 ) { idx = 0; }
    out[i] = idx;
    idx++;
  }
BYE:
  return(status);
}
