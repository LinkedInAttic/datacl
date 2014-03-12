#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_mmad___XTYPE1_____XTYPE2__(
    __ITYPE1__ *f1, 
    long long nR1, 
    __ITYPE2__ *f2, 
    long long nR2, 
    __ITYPE1__ *ptr_minval,
    __ITYPE1__ *ptr_maxval,
    long long *ptr_sum
    )
// STOP FUNC DECL
{
  int status = 0;
  __ITYPE1__ minval = *ptr_minval;
  __ITYPE1__ maxval = *ptr_maxval;
  long long sum = *ptr_sum;
  for ( long long i = 0; i < nR2; i++ ) {
    __ITYPE2__ f1idx = f2[i];
#ifdef DEBUG
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
#endif
    __ITYPE1__ f1val = f1[f1idx];
    if ( f1val < minval ) { minval = f1val; }
    if ( f1val > maxval ) { maxval = f1val; }
    sum += (long long)f1val;
  }
  cBYE(status);
  *ptr_minval = minval;
  *ptr_maxval = maxval;
  *ptr_sum    = sum;
BYE:
  return(status);
}
