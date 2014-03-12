#include <stdbool.h>
// START FUNC DECL
void
cmp_eq_mult_scalar_log___INTYPE__( 
			      __INTYPE2__ *in1, 
			      long long n1, 
			      __INTYPE2__ *in2, 
			      int n2, 
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    bool found = false;
    __INTYPE2__ inval = in1[i];
    int ub = n2;
    int lb = 0;
    int probe;
    __INTYPE2__ val_to_cmp;
    bool is_last_iter = false;
    for ( ;  ub > lb;  ) {
      probe = (lb + ub) / 2 ;
      if ( ( probe == lb ) || ( probe == ub ) )  { 
	is_last_iter = true;
      }
      val_to_cmp = in2[probe];
      if ( inval == val_to_cmp ) { 
	found = true;
	break;
      }
      else if ( inval > val_to_cmp ) {
	lb = probe;
      }
      else if ( inval < val_to_cmp ) { 
	ub = probe;
      }
      else { /* control cannot come here */ }
      if ( is_last_iter ) { break; }
    }
    *out++ = found;
  }
}
