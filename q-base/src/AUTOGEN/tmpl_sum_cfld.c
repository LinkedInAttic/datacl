#include <stdio.h>
#include "../src/constants.h"
#include "../src/macros.h"
// START FUNC DECL
int
sum_cfld___XTYPE__(
		__ITYPE__ *in,
		long long n,
		char *cfld,
		long long *ptr_rslt
		)
// STOP FUNC DECL
{
  int status = 0;
  register char  *cptr = cfld;
  register long long rslt = 0;
#pragma simd reduction(+:rslt)
  for ( long long i = 0; i < n; i++ ) {
    if ( cfld[i] == FALSE ) { continue; }
    rslt += in[i];
  }
  *ptr_rslt = rslt;
BYE:
  return(status);
}
