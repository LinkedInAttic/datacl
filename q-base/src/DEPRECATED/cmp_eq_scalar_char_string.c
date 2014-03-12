#include <stdbool.h>
#include <string.h>
#include "constants.h"
// START FUNC DECL
void
cmp_eq_scalar_char_string(
    char *X,    /* input string */
    const char *nn_X, /* nn field for input string */
    const int *sz_X,  /* sz field for input string */
    const int n,      /* number of values */
    const char *scalar, /* scalar string value */
    bool *op /* output boolean */
    )
// STOP FUNC DECL
{
  for ( int i = 0; i < n; i++ ) { 
    if ( ( nn_X != NULL ) && ( nn_X[i] == FALSE ) ) { 
      op[i] = FALSE;
    }
    else {
      if ( strcmp(X, scalar) == 0 ) { 
        op[i] = TRUE;
      }
      else {
	op[i] = FALSE;
      }
    }
    X += sz_X[i];
  }
}
