#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "read_meta_data.h"
#include "orig_meta_globals.h"

int 
main(
    int argc,
    char **argv
    )	
{
  int status = 0;
  FLD_TYPE *flds = NULL;
  int n_flds;

  if ( argc != 2 ) { go_BYE(-1); }
  status = read_meta_data(argv[1], &flds, &n_flds);
  fprintf(stdout, "n_flds = %d \n", n_flds);
BYE:
  free_if_non_null(flds);
  return(status);
}
