#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "assign_I8.h"
// last review 9/9/2013
// START FUNC DECL
int
zero_after_n(
    char *X,
    long long nR,
    long long one_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  long long *Y = NULL; long long nY, i, cnt = 0;

  if ( X == NULL ) { go_BYE(-1); }
  nY = nR / NUM_BITS_IN_I8;
  if ( ( nY / NUM_BITS_IN_I8 ) != nR )  { nY++; }
  Y = (long long *)X;

  for ( i = 0; i < nY; i++ ) {
    cnt +=  __builtin_popcountll(Y[i]);
    if ( cnt >= one_cnt ) { 
      i++;
      break; 
    }
  }
  assign_const_I8(Y, (nY-i), 0); // set rest to 0 
BYE:
  return status ;
}


