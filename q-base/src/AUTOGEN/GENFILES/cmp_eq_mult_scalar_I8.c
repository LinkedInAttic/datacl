#include "constants.h"
#include "dbg_cmp_eq_mult_scalar_I8.h"
#include "opt_cmp_eq_mult_scalar_I8.h"
// START FUNC DECL
void
cmp_eq_mult_scalar_I8( 
			      long long *invals, 
			      long long n1, 
			      long long *svals,
			      int n_svals,
			      long long *hashvals, 
			      int n,
			      int a,
			      int b,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  if ( n_svals <= OPT_NUM_SCALARS ) { 
    dbg_cmp_eq_mult_scalar_I8(invals, n1, svals, n_svals, out);
  }
  else {
    opt_cmp_eq_mult_scalar_I8(invals, n1, hashvals, n, a, b, out);
  }
}
