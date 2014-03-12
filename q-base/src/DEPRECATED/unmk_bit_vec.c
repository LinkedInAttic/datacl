// INTEL OPTIMIZE
#include <stdio.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int 
unmk_bit_vec(
    const unsigned long long *in, 
    long long nR, 
    char *out
    )
// STOP FUNC DECL
{
  int status = 0;

  if ( in  == NULL ) { go_BYE(-1); }
  if ( out == NULL ) { go_BYE(-1); }
  if ( nR <= 0 ) { go_BYE(-1); }

  long long inidx = 0;
  int counter = 0;
  int shiftby = 63;
  unsigned long long ltemp;
  unsigned long long lval = in[inidx];
  unsigned long long mask = 9223372036854775808;
  for ( long long i = 0; i < nR; i++ ) { 
    ltemp  = lval & mask;
    ltemp = ltemp >> shiftby;
    if ( ( ltemp != 0 )  && ( ltemp != 1 ) ) { go_BYE(-1); }
    out[i] = ltemp;
    mask = mask >> 1;
    if ( counter == 64 ) {
      counter = 0;
      shiftby = 63;
      lval = in[inidx];
      inidx++;
      mask = (unsigned long long)9223372036854775808;
    }
    else {
      counter++;
      shiftby--;
    }
  }
BYE:
  return(status);
}

