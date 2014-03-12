#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "parse_scalar_vals.h"

int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  int *Y = NULL; int nY = 0;

  if ( argc != 2 ) { go_BYE(-1); }
  status = parse_scalar_vals(argv[1], &Y, &nY);
  for ( int i = 0; i < nY; i++ ) { 
    fprintf(stdout, "%d\n", Y[i] );
  }
BYE:
  free_if_non_null(Y);
  return(status);
}
