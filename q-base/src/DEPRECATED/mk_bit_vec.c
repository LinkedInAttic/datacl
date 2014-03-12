// INTEL OPTIMIZE
#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "mk_bit_vec.h"
// START FUNC DECL
int
mk_bit_vec(
    const char *X,
    long long nX,
    unsigned long long *out
    )
// STOP FUNC DECL
{
  int status = 0;
  int counter = 0;
  int shiftby = 63;
  unsigned long long lval = 0;
  unsigned long long ltemp;
  long long outidx = 0;
  for ( long long i = 0; i < nX; i++ ) { 
    char c = X[i];
    if ( ( c != 0 ) && ( c != 1 ) ) { go_BYE(-1); }
    ltemp = c;
    ltemp = ltemp << shiftby;
    lval = lval | ltemp;
    if ( counter == 64 ) { 
      out[outidx++] = lval;
      lval = 0;
      counter = 0;
      shiftby = 63;
    }
    else {
      counter++;
      shiftby--;
    }
    // fprintf(stderr, "i = %d, c = %d, ltemp = %lld, lval = %lld \n",
//	i, c, ltemp,lval);
  }
  // handle the last guy  TODO: Check that this is correct
  if ( counter != 0 ) {
    out[outidx++] = lval;
  }

BYE:
  return(status);
}
