#include <stdio.h>
#include "constants.h"
#include "macros.h"

/* TODO P2: See if there is a library function call for this */
// START FUNC DECL
int
conv_B_to_I1(
    char *X,
    long long nX,
    char * restrict Y
    )
// STOP FUNC DECL
{
  int status = 0;
  unsigned char uix;
  /* Need to deal with fact that nX may not be a multiple of 8  */
  long long xindex = 0;
  long long yindex = 0;

  long long n = ( nX / 8 ) * 8;
  for ( yindex = 0; yindex < n; ) {
    uix = X[xindex];
    for ( int j = 0; j < 8; j++ ) { 
      if ( ( uix & 0x01 ) == 1 ) {
        Y[yindex++] = 1;
      }
      else { 
        Y[yindex++] = 0;
      }
      uix = uix >> 1;
    }
    xindex++;
  }
  if ( xindex != yindex/8 ) { go_BYE(-1); }
  int leftover = nX - n;
  for ( ; yindex < nX; ) {
    uix = X[xindex];
    for ( int j = 0; j < leftover; j++ ) { 
      if ( ( uix & 0x01 ) == 1 ) {
        Y[yindex++] = 1;
      }
      else { 
        Y[yindex++] = 0;
      }
      uix = uix >> 1;
    }
    xindex++;
  }
  if ( yindex != nX ) { go_BYE(-1); }
  /* TODO: P2 Any more invariants? */

BYE: 
  return status ;
}

