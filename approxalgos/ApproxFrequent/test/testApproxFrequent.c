#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "approx_frequent.h"
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include <malloc.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main (
	  int argc,
	  char **argv
	  )
{

  int status = 0;

  int *x = NULL;
  char *X = NULL; size_t nX = 0;

  int *y = NULL;
  int *f = NULL;
  
  char * cfld = NULL; size_t nY = 0;

  if ( argc != 2 && argc != 3 ) { go_BYE(-1); }
  /* argc == 2 would mean just having the input binary file ( cfld will be assumed to be NULL, argc == 3 would mean that a corresponding cfld binary file is also included */

  char * infile = argv[1];
  status = rs_mmap(infile, &X, &nX, 0); cBYE(status);
  x = (int *)X;
  if ( x == NULL ) { go_BYE(-1); }
  long long siz = nX / sizeof(int);	   

  if ( argc == 3 ) { 
    char * cfldfile = argv[2];
    status = rs_mmap(cfldfile, &cfld, &nY, 0); cBYE(status);
    if  ( cfld == NULL ) { go_BYE(-1); }
  }

  //-----------------------------------------------------------------------

  long long min_freq = 400; 
  long long err = 100; /* has to be strictly less than min_freq */
  if ( err >= min_freq || err <= 0 ) { go_BYE(-1); }

  // Output (y,f) properties:
  // (i)   any element occuring greater than or equal to min_freq times in x will definitely be listed in y. 
  // (ii)  the frequency of any element occuring greater than or equal to min_freq times will be calculated up to a maximum error of err, and will be listed in f (corresponding to the id y). 
  // (iii) no element occuring less than (min_freq - err) times in x will be listed in the output y.

  int estimate_is_good;

  long long out_siz = siz/(min_freq-err); 
  /* max possible output elements ( all elements with estimated frequency greater than or equal to (min_freq-err) will be output): only (siz/(min_freq-err)) number of elements can occur greater than or equal to (min_freq-err) times */

  /* pre-allocating memory */
  y = (int *) malloc( out_siz * sizeof(int) );
  return_if_malloc_failed(y);
  f = (int *) malloc( out_siz * sizeof(int) );
  return_if_malloc_failed(f);

  long long len = 0; /* number of entries written in the outputs y and f by the function approx_frequent() */
    
  status = approx_frequent(x, cfld, siz, min_freq, err, y, f, out_siz, &len, &estimate_is_good); cBYE(status);

  if ( estimate_is_good == 1 ) { 

    for ( long long ii = 0; ii < len; ii++ ) {
    printf("\n(y, f):" ANSI_COLOR_GREEN"(%d, %d)"ANSI_COLOR_RESET, y[ii], f[ii]);
    }
  }
  else {
    printf("\n Need too much memory to do the calculations. Try one of the following: (i) In approx_quantile() function, increase MAX_SZ if you know you have more RAM (ii) Increase err, which will result in more approximation but computations can be done with lesser memory" );
  }

  
BYE:

  rs_munmap(X, nX);
  if ( cfld != NULL ) {
    rs_munmap(cfld, nY);
  }

  free_if_non_null(y);
  free_if_non_null(f);

  return(status);
}
