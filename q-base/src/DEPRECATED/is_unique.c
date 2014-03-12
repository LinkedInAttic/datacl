#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "qtypes.h"
#include "macros.h"
#include "fsize.h"
#include "is_unique.h"
#include "sort_asc_int.h"

// START FUNC DECL
int
is_unique(
    int *X,
    int nX,
    bool *ptr_is_unique
    )
// STOP FUNC DECL
{
  int status = 0;
  int *Y = NULL;

  if ( X == NULL ) { go_BYE(-1); }
  if ( nX < 0 ) { go_BYE(-1); }
  *ptr_is_unique = true; 
  if ( nX <= 1 ) { return(0); }
  Y = (int *)malloc(nX * sizeof(int));
  return_if_malloc_failed(Y);
  for ( int i = 0; i < nX; i++ ) { 
    Y[i] = X[i];
  }
  qsort(Y, nX, sizeof(int), sort_asc_int);
  for ( int i = 1; i < nX; i++ ) { 
    if ( Y[i] == Y[i-1] ) {
      *ptr_is_unique = false; 
      break;
    }
  }
BYE:
  return(status);
}
