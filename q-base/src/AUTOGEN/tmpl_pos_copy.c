#include <stdio.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_copy___XTYPE1_____XTYPE2__(
    __ITYPE1__ *f1, 
    long long nR1, 
    __ITYPE2__ *f2, 
    long long nR2, 
    __ITYPE1__ *f3
    )
// STOP FUNC DECL
{
  int status = 0;
  for ( long long i = 0; i < nR2; i++ ) {
    __ITYPE2__ f1idx = f2[i];
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
    __ITYPE1__ ival = f1[f1idx];
    f3[i] = ival;
  }
BYE:
  return(status);
}

