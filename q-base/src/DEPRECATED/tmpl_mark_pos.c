#include <stdio.h>
#include "../src/constants.h"
#include "../src/macros.h"
// START FUNC DECL
int
mark_pos___XTYPE__(
		__ITYPE__ *in,
		long n,
		char * restrict op,
		int n_out
		)
// STOP FUNC DECL
{
  int status = 0;
  __ITYPE__ prev_idxI4 = -1;
  for ( long long i = 0; i < n; i++ ) {
    __ITYPE__ idx = in[i];
    int idxI4 = (int )idx;
    if  ( prev_idxI4 == idxI4 ) { 
      continue;
    }
#undef DEBUG
#ifdef DEBUG
    if ( ( idx < 0 ) || ( idx >= n_out ) ) { go_BYE(-1); }
#endif
    op[idxI4] = 1;
    prev_idxI4 = idxI4;
  }
#ifdef DEBUG
 BYE:
#endif
  return(status);
}
