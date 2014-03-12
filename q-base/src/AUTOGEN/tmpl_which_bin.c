#include <stdbool.h>
// START FUNC DECL
void
which_bin___XTYPE__(
		    __ITYPE__ inval,
		    __ITYPE__ *lb_vals,
		    __ITYPE__ *ub_vals,
		    int nvals,
		    __ITYPE__ *ptr_idx,
		    bool *ptr_exists
		    )
// STOP FUNC DECL
{
  __ITYPE__ idx = -1;
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
