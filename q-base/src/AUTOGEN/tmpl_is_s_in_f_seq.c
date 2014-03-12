#include <stdio.h>
// START FUNC DECL
void
is_s_in_f_seq___XTYPE__(
    __ITYPE__ *X,
    char *nn_X,
    __ITYPE__ val,
    long long nR,
    long long *ptr_idx
    )
// STOP FUNC DECL
{
  // TODO P2: Cilk-ify? Can vectorization help?
  *ptr_idx = -1;
  for ( long long i = 0; i < nR; i++ ) {
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) { continue; }
    if ( X[i] == val ) { 
      *ptr_idx = i;
      break;
    }
  }
}

