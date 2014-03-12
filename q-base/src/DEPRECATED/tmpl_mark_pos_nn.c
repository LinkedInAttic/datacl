#include <stdio.h>
#include "../src/constants.h"
#include "../src/macros.h"
// START FUNC DECL
int
mark_pos_nn___XTYPE__(
		__ITYPE__ *in,
		long long n,
		char *cfld,
		char * restrict out,
		int n_out
		)
// STOP FUNC DECL
{
  int status = 0;
  int prev_idxI4 = -1;
  register char  *cptr = cfld;
  for ( long long i = 0; i < n; i++ ) {
    if ( *cptr++ == FALSE ) { continue; }
    __ITYPE__ idx = in[i];
    int idxI4 = (int)idx;
#undef DEBUG
#ifdef DEBUG
    if ( ( idx < 0 ) || ( idx >= n_out ) ) { go_BYE(-1); }
#endif
    out[idxI4] = TRUE;
    prev_idxI4 = idxI4;
  }
#ifdef DEBUG
 BYE:
#endif
  return(status);
}
