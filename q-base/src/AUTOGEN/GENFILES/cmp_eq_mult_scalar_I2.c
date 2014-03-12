#include "constants.h"
#include "dbg_cmp_eq_mult_scalar_I2.h"
#include "opt_cmp_eq_mult_scalar_I2.h"
// START FUNC DECL
void
cmp_eq_mult_scalar_I2( 
			      short *invals, 
			      long long n1, 
			      short *svals,
			      int n_svals,
			      short *hashvals, 
			      int n,
			      int a,
			      int b,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  if ( n_svals <= OPT_NUM_SCALARS ) { 
    dbg_cmp_eq_mult_scalar_I2(invals, n1, svals, n_svals, out);
  }
  else {
    opt_cmp_eq_mult_scalar_I2(invals, n1, hashvals, n, a, b, out);
  }
}
