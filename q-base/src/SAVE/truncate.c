#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
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
  FILE  *fp = NULL;
  char *infile = NULL;
  long long offset, retval;
  int filedes;
  char *X = NULL; size_t nX = 0;

  if ( argc != 2 ) {  go_BYE(-1); }
  infile = argv[1];
  fp = fopen(infile, "rb");
  return_if_fopen_failed(fp, infile, "rb");
  /*
  retval = lseek(filedes, 0, SEEK_END);
  fprintf(stderr, "retval = %d \n", retval);
  */
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  fprintf(stderr, "nX = %lld \n", nX);
  for ( long long i = nX -1; i >= 0; i-- ) { 
    if ( X[i] == '\n' ) { 
      offset = i+1;
      fprintf(stderr, "offset = %lld \n", offset);
      break;
    }
  }
  rs_munmap(X, nX);
  fclose_if_non_null(fp);
  // open for truncate
  filedes = open(infile, O_TRUNC, O_WRONLY);

  // truncate 
  ftruncate(filedes, offset);

BYE:
  rs_munmap(X, nX);
  fclose_if_non_null(fp);
  return(status);
}

