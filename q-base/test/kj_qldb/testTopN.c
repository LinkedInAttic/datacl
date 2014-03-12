#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <malloc.h>
#include <fcntl.h>
#include "constants.h"
#include "macros.h"
#include "top_n.h"
#include "mmap.h"

int main (
	  int argc,
	  char ** argv
	  )
{
  
  int status = 0;

  int *x = NULL;
  char *X = NULL; size_t nX = 0;
 
  if ( argc != 2 ) { assert(0); }

  char * infile = argv[1];  /* input file with freq */

  status = rs_mmap(infile, &X, &nX, 0); 
  cBYE(status);
 
  x = (int *)X;
  if ( x == NULL ) { go_BYE(-1); }

  long long siz = nX / sizeof(int);

  int n = 3;
  long long * z = malloc ( n * sizeof(long long) );

  status = top_n (x, NULL, siz, n, z);
  cBYE(status);


 BYE:

  rs_munmap(X, nX);

  return (status);

}




