#include <stdbool.h>
// START FUNC DECL
void
which_bin_I4(
		    int inval,
		    int *lb_vals,
		    int *ub_vals,
		    int nvals,
		    int *ptr_idx,
		    bool *ptr_exists
		    )
// STOP FUNC DECL
{
  int idx = -1;
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
