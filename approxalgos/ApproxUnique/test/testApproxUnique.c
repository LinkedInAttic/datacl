#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "approx_unique.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main (
	  int argc,
	  char ** argv
	  )
{

  int status = 0;

  int *x = NULL;
  char *X = NULL; size_t nX = 0;
  char *cfld = NULL; size_t nY = 0;

  if ( (argc != 2) && (argc != 3) ) { go_BYE(-1); }
  /* argc == 2 would mean just having the input binary file ( cfld will be assumed to be NULL, argc == 3 would mean that a corresponding cfld binary file is also included */

  char * infile = argv[1];      
  status = rs_mmap(infile, &X, &nX, 0); cBYE(status);
  long long siz = nX / sizeof(int);	   
  x = (int *)X;
  if (x == NULL ) { go_BYE(-1); }

  if ( argc == 3 ) {
    char * cfldfile = argv[2];
    status = rs_mmap(cfldfile, &cfld, &nY, 0); cBYE(status);
    if ( cfld == NULL ) { go_BYE(-1);}
  }

  //-----------------------------------------------------------------------

  long long y;
  double estimate_accuracy;
  int estimate_is_good;

  status = approx_unique(x, cfld, siz, &y, &estimate_accuracy, &estimate_is_good); cBYE(status);

  if ( estimate_is_good == 1 ) {

    printf("\nEstimated total number of unique entries:" ANSI_COLOR_GREEN" %lld"ANSI_COLOR_RESET" +/- : "ANSI_COLOR_GREEN "%.2f percent"ANSI_COLOR_RESET"\n", y, estimate_accuracy);

  }
  else if ( estimate_is_good == -2 ) {

    printf("\nToo few entries. Use (val, count) based methods to estimate cardinality\n");

  }
  else if ( estimate_is_good == -3 ) {

    printf("\nHash function produced a value which is divisible by 2^44. This can be because of 2 reasons: (1) Bad luck - retry and you should be fine (2) Cardinality of the set greater than 1 trillion - use a 128 bit hash function in approx_unique(), needs some editing in approx_unique() and determine_b_k()\n");

  }

 BYE:

  rs_munmap(X, nX);
  if ( cfld != NULL ) {
    rs_munmap(cfld, nY);
  }

  return(status);
  
}
