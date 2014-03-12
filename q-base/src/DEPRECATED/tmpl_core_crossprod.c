#include <stdio.h>
// START FUNC DECL
int
core_crossprod___IN11_____IN21__(
				 char *f1_X,
				 long long nR1,
				 char *f2_X,
				 long long nR2,
				 char *Y1,
				 char *Y2,
				 long long nR3
				 )
// STOP FUNC DECL
{
  int status = 0;
  __IN12__ i1val, *iptr1, *optr1;
  __IN22__ i2val, *iptr2, *optr2;
  long long t3_idx = 0;

  iptr1 = (__IN12__ *)f1_X;
  iptr2 = (__IN22__ *)f2_X;
  optr1 = (__IN12__ *)Y1;
  optr2 = (__IN22__ *)Y2;
  for ( long long i1 = 0; i1 < nR1; i1++ ) { 
    i1val = iptr1[i1];
    for ( long long i2 = 0; i2 < nR2; i2++ ) { 
      i2val = iptr2[i2];
#ifdef DEBUG
      if ( t3_idx >= nR3 ) { return -1;  }
#endif
      optr1[t3_idx] = i1val;
      optr2[t3_idx] = i2val;
      t3_idx++;
    }
  }
  return status;
}
