#include <stdbool.h>
// START FUNC DECL
void
which_bin_I8(
		    long long inval,
		    long long *lb_vals,
		    long long *ub_vals,
		    int nvals,
		    long long *ptr_idx,
		    bool *ptr_exists
		    )
// STOP FUNC DECL
{
  long long idx = -1;
  bool exists = false;
  for ( int i = 0; i < nvals; i++ ) { 
    if ( ( inval >= lb_vals[i] ) &&  ( inval <= ub_vals[i] ) ) {
      idx = i;
      exists = true;
      break;
    }
  }
  *ptr_idx = idx;
  *ptr_exists = exists;
}
