#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "mk_file.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *X = NULL; size_t nX = 0, alt_nX = 0;
  char *Y = NULL; size_t nY = 0;
  char *filename = NULL;
  char *out_filename = NULL;
  FILE *ofp = NULL;

  if ( argc != 3 ) { go_BYE(-1); }
  filename = argv[1];
  out_filename = argv[2];
  if ( strcmp(filename, out_filename) == 0 ) { go_BYE(-1); }
  status = rs_mmap(filename, &X, &nX, 0); cBYE(status);
  Y = X + nX - 1;
  alt_nX = nX;
  for ( ; *Y != '\n'; Y-- ) { 
    alt_nX--;
  }
  for ( char *Z = Y; Z < X + nX; Z++ ) { 
    fprintf(stderr, "%c", *Z);
  }
  fprintf(stderr, "\n");
  status = mk_file(out_filename, alt_nX);
  status = rs_mmap(out_filename, &Y, &nY, 1); cBYE(status);
  if ( nY != alt_nX ) { go_BYE(-1); }
  memcpy(Y, X, nY);

BYE:
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  return(status);
}
