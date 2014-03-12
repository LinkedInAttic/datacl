#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
pos_mmad___XTYPE1_____XTYPE2___nn(
    __ITYPE1__ *f1, 
    char *nn_f1,
    long long nR1, 
    __ITYPE2__ *f2, 
    char *nn_f2,
    long long nR2, 
    __ITYPE1__ *ptr_minval,
    __ITYPE1__ *ptr_maxval,
    long long *ptr_sum,
    long long *ptr_cnt
    )
// STOP FUNC DECL
{
  int status = 0;
  __ITYPE1__ minval = *ptr_minval;
  __ITYPE1__ maxval = *ptr_maxval;
  long long sum = *ptr_sum; 
  long long cnt = *ptr_cnt; 
  for ( long long i = 0; i < nR2; i++ ) {
    if ( ( nn_f2 != NULL ) && ( nn_f2[i] == FALSE ) ) { continue; }
    __ITYPE2__ f1idx = f2[i];
    if ( ( nn_f1 != NULL ) && ( nn_f1[f1idx] == FALSE ) ) { continue; }
    if ( ( f1idx < 0 ) || ( f1idx >= nR1 ) ) { go_BYE(-1); }
    __ITYPE1__ f1val = f1[f1idx];
    if ( f1val < minval ) { minval = f1val; }
    if ( f1val > maxval ) { maxval = f1val; }
    sum += (long long)f1val;
    cnt++;
  }
  *ptr_minval = minval;
  *ptr_maxval = maxval;
  *ptr_sum    = sum;
  *ptr_cnt    = cnt;
BYE:
  return(status);
}

