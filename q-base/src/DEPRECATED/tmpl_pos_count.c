#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_count___XTYPE1_____XTYPE2__(
    __ITYPE1__ *f1, 
    long long nR1, 
    __ITYPE2__ *f2, 
    long long nR2, 
    int *f3,
    long long nR3
    )
// STOP FUNC DECL
{
  int status = 0;
  for ( long long i = 0; i < nR2; i++ ) {
    __ITYPE2__ f1idx = f2[i];
#ifdef DEBUG
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
#endif
    __ITYPE1__ f3idx = f1[f1idx];
#ifdef DEBUG
    if ( ( f3idx < 0 ) || ( f3idx >= nR3 ) ) { go_BYE(-1); }
#endif
    f3[f3idx]++;
  }
#ifdef DEBUG
BYE:
#endif
  return(status);
}

