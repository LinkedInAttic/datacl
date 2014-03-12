#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile, *outfile;
  FILE *fp = NULL;
  char *X = NULL; size_t nX;

  if ( argc != 3 ) { go_BYE(-1); }
  infile  = argv[1];
  outfile = argv[2];
  if ( strcmp(infile, outfile) == 0 ) { go_BYE(-1); }
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  fp = fopen(outfile, "wb");
  fwrite(X, 1, nX, fp);
  fclose_if_non_null(fp);
BYE:
  return status ;
}
