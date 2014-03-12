#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "aux_fld_meta.h"
#include "parse_attrs.h"

int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char fldtype[MAX_LEN_FLD_TYPE];
  int n_sizeof;

  zero_string(fldtype, MAX_LEN_FLD_TYPE);
  if ( argc != 2 ) { go_BYE(-1); }
  status = parse_attrs(argv[1], &n_sizeof, fldtype);
BYE:
  return(status);
}
