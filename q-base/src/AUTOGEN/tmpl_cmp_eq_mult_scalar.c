#include "constants.h"
#include "dbg_cmp_eq_mult_scalar___XTYPE__.h"
#include "opt_cmp_eq_mult_scalar___XTYPE__.h"
// START FUNC DECL
void
cmp_eq_mult_scalar___XTYPE__( 
			      __ITYPE__ *invals, 
			      long long n1, 
			      __ITYPE__ *svals,
			      int n_svals,
			      __ITYPE__ *hashvals, 
			      int n,
			      int a,
			      int b,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  if ( n_svals <= OPT_NUM_SCALARS ) { 
    dbg_cmp_eq_mult_scalar___XTYPE__(invals, n1, svals, n_svals, out);
  }
  else {
    opt_cmp_eq_mult_scalar___XTYPE__(invals, n1, hashvals, n, a, b, out);
  }
}
