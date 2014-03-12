#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "sortbindmp.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile = NULL;
  char *fld_spec = NULL;
  char *sort_order = NULL;

  if ( argc != 4 ) {
    fprintf(stderr, "Usage is %s <infile> <fld_spec> <sort_order> \n", argv[0]);

    go_BYE(-1);
  }
  infile = argv[1];
  fld_spec = argv[2];
  sort_order = argv[3];

  status = sortbindmp(infile, fld_spec, sort_order);
  cBYE(status);
BYE:
  return(status);
}

