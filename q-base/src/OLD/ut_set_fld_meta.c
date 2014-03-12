#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "aux_fld_meta.h"
#include "set_fld_meta.h"

int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  FLD_META_TYPE fld_meta;

  zero_fld_meta(&fld_meta);
  if ( argc != 2 ) { go_BYE(-1); }
  status = set_fld_meta(argv[1], &fld_meta);
  cBYE(status);
  status = pr_fld_meta(fld_meta);
  cBYE(status);
BYE:
  return(status);
}
