#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"

/* Given a binary file, it creates an ascii file 
   Inputs 
   (1) Input file (binary)
   (2) fldtype, can be int or long long
   Output
   (1) Output file (ascii)

*/

#define I_INT 1
#define I_LONG_LONG 2
#define I_FLOAT 3
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *infile = NULL;
  char *outfile = NULL;
  char *fldtype = NULL; int ifldtype;
  FILE *ofp = NULL;
  char *endptr = NULL; int num_cols;
  int rec_size, num_rows, *iptr = NULL; long long *llptr = NULL;
  float *fptr = NULL; 
  char *X = NULL; size_t nX;
  int itemp;

  if ( argc != 5 ) { 
    fprintf(stderr, "Usage is %s <infile> <fldspec> <numcols> <outfile>\n", 
	argv[0]);
    go_BYE(-1);
  }
  infile  = argv[1];
  fldtype = argv[2];
  num_cols = strtol(argv[3], &endptr, 10);
  if ( num_cols < 1 ) { go_BYE(-1); }
  outfile = argv[4];
  if ( strcmp(infile, outfile) == 0 ) { go_BYE(-1); }
  if ( *infile == '\0' ) { go_BYE(-1); }

  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);

  if ( *outfile != '\0' ) {
    ofp = fopen(outfile, "w");
    return_if_fopen_failed(ofp, outfile, "w");
  }
  else {
    ofp = stdout;
  }
  if ( strcasecmp(fldtype, "int") == 0 ) {
    ifldtype = I_INT;
    rec_size = sizeof(int) * num_cols;
    iptr = (int *)X;
  }
  else if ( strcasecmp(fldtype, "float") == 0 ) {
    ifldtype = I_FLOAT;
    rec_size = sizeof(float) * num_cols;
    fptr = (float *)X;
  }
  else if ( strcasecmp(fldtype, "long long") == 0 ) {
    ifldtype = I_LONG_LONG;
    rec_size = sizeof(long long) * num_cols;
    llptr = (long long *)X;
  }
  else { go_BYE(-1); }
  num_rows = nX / rec_size;
  if ( ( num_rows * rec_size) != nX ) { go_BYE(-1); }

  for ( int i = 0; i < num_rows; i++ ) { 
    switch ( ifldtype ) { 
      case I_INT : 
	for ( int j = 0; j < num_cols; j++ ) {
	  if ( j > 0 ) {
	    fprintf(ofp, ",");
	  }
	  fprintf(ofp, "%d", *iptr++);
	}
	fprintf(ofp, "\n");
	break;
      case I_FLOAT : 
	for ( int j = 0; j < num_cols; j++ ) {
	  if ( j > 0 ) {
	    fprintf(ofp, ",");
	  }
	  fprintf(ofp, "%f", *fptr++);
	}
	fprintf(ofp, "\n");
	break;
      case I_LONG_LONG : 
	for ( int j = 0; j < num_cols; j++ ) {
	  if ( j > 0 ) {
	    fprintf(ofp, ",");
	  }
	  fprintf(ofp, "%lld", *llptr++);
	}
	fprintf(ofp, "\n");
	break;
      default :
	go_BYE(-1);
	break;
    }
    itemp = i;
  }
 BYE:
  rs_munmap(X, nX);
  if ( *outfile != '\0' ) {
    fclose_if_non_null(ofp);
  }
  return(status);
}
