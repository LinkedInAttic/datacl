#include <stdio.h>
#include "macros.h"
// START FUNC DECL
int 
intersection_I8(
    long long *f1ptr,
    long long nR1,
    long long *f2ptr,
    long long nR2,
    long long *f3ptr,
    long long *ptr_nR3
    )
// STOP FUNC DECL
{
  int status = 0;
  long long nR3 = 0;
  long long f1val, f2val;
  long long *endf1ptr = f1ptr + nR1;
  long long *endf2ptr = f2ptr + nR2;

  for ( ; ; ) {
    if ( f1ptr == endf1ptr ) { break; }
    if ( f2ptr == endf2ptr ) { break; }
    f1val = *f1ptr;
    f2val = *f2ptr;
    if ( f1val == f2val ) { 
	*f3ptr = f1val;
	f3ptr++;
	nR3++;
	if ( nR3 > (nR1+nR2) ) { go_BYE(-1); }
	f1ptr++;
	f2ptr++;
      }
      else if ( f1val < f2val ) { 
	f1ptr++;
      }
      else if ( f2val < f1val ) { 
	f2ptr++;
      }
    }
  *ptr_nR3 = nR3;
BYE:
  return(status);
}
