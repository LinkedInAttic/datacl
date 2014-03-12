#include <stdbool.h>
#include "cmp_eq_mult_scalar_simple_I1.h"
#include "cmp_eq_mult_scalar_simple_I2.h"
#include "cmp_eq_mult_scalar_simple_I4.h"
#include "cmp_eq_mult_scalar_simple_I8.h"
#include "cmp_eq_mult_scalar_log_I1.h"
#include "cmp_eq_mult_scalar_log_I2.h"
#include "cmp_eq_mult_scalar_log_I4.h"
#include "cmp_eq_mult_scalar_log_I8.h"
// START FUNC DECL
void
cmp_eq_mult_scalar___INTYPE__( 
			      __INTYPE2__ *in1, 
			      long long n1, 
			      __INTYPE2__ *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  if ( n2 <= 8 ) {
    cmp_eq_mult_scalar_simple___INTYPE__( in1, n1, in2, n2, out);
  }
  else 
    cmp_eq_mult_scalar_log___INTYPE__( in1, n1, in2, n2, out);
}
