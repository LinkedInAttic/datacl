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
#include "count_vals.h"
#include "mmap.h"

int main (
	  int argc,
	  char ** argv
	  )
{
  
  int status = 0;

  int *x = NULL;
  char *X = NULL; size_t nX = 0;

  FILE * fp1 = NULL; 
  FILE * fp2 = NULL;
 
  if ( argc != 4 ) { assert(0); }

  char * infile = argv[1];     /* input file */
  char * outfile1 = argv[2];   /* output file with the ids */
  char * outfile2 = argv[3];   /* output file with the corr. frequencies */

  fp1 = fopen (outfile1, "wb");
  return_if_fopen_failed(fp1, outfile1, "wb");

  fp2 = fopen (outfile2, "wb");
  return_if_fopen_failed(fp2, outfile2, "wb");

  status = rs_mmap(infile, &X, &nX, 0); 
  cBYE(status);
 
  x = (int *)X;
  if ( x == NULL ) { go_BYE(-1); }

  long long siz = nX / sizeof(int);
  long long out_siz;

  status = count_vals_maps (x, NULL, siz, fp1, fp2, &out_siz);
  cBYE(status);

  printf("\nNumber of unique entries: %lld", out_siz);
  fflush(stdout);

  /*status = count_vals_ldb (x, NULL, siz, fp1, fp2, &out_siz);
  cBYE(status);

  printf("\nNumber of unique entries: %lld", out_siz);
  fflush(stdout);*/


 BYE:

  fclose_if_non_null(fp1);
  fclose_if_non_null(fp2);
  rs_munmap(X,nX);

  return (status);

}




