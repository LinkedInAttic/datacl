#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "parse_scalar_vals.h"

/* Given a string X of the form n1:n2:n3, return an array of integers. 
   If X is empty string, then function returns wth nY = 0. If any of the
   strings are not an integer, then error. */

// START FUNC DECL
int
parse_scalar_vals(
		  char *X,
		  int **ptr_Y,
		  int *ptr_nY
		  )
// STOP FUNC DECL
{
  int status = 0;
  int *Y = NULL;
  int i, nY = 0;
  char *cpy_X = NULL, *bak_cpy_X = NULL, *endptr = NULL, *cptr = NULL;

  if ( X == NULL ) { go_BYE(-1); }
  if ( *X == '\0' ) { goto BYE; }
  bak_cpy_X = cpy_X = malloc((strlen(X)+1) * sizeof(char));
  return_if_malloc_failed(cpy_X);
  strcpy(cpy_X, X);
  nY = 1;
  for ( i = 0, cptr = cpy_X; *cptr != '\0'; cptr++ ) { 
    if ( *cptr == ':' ) { 
      nY++;
    }
  }
  Y = malloc(nY * sizeof(int));
  return_if_malloc_failed(Y);
  for ( int i = 0; i < nY; i++ ) { 
    cptr = strsep(&cpy_X, ":");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    Y[i] = strtol(cptr, &endptr, 10);
    if ( *endptr != '\0' ) { 
      nY = 0;
      free_if_non_null(Y);
      go_BYE(-1);
    }
  }
 BYE:
  free_if_non_null(bak_cpy_X);
  *ptr_Y = Y;
  *ptr_nY = nY;
  return(status);
}
