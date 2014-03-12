#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"

/* Given a single column integer file, it converts it into binary.
   Inputs 
   (1) Input file
   (2) fldtype, can be int or long long
   Output
   (1) Output file

   * backslashes and dealing with eoln within the quotes */

#define MAXLINE 32

#define I_INT 1
#define I_LONG_LONG 2
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
  FILE *ifp = NULL;
  FILE *ofp = NULL;
  char *cptr, *endptr;
  char line[MAXLINE];
  int itemp; long long ltemp;

  if ( argc != 4 ) { go_BYE(-1); }
  infile  = argv[1];
  fldtype = argv[2];
  outfile = argv[3];

  if ( strcmp(infile, outfile) == 0 ) { go_BYE(-1); }

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(outfile, "wb");
  return_if_fopen_failed(ofp, outfile, "wb");
  if ( strcasecmp(fldtype, "int") == 0 ) {
    ifldtype = I_INT;
  }
  else if ( strcasecmp(fldtype, "long long") == 0 ) {
    ifldtype = I_LONG_LONG;
  }
  else { go_BYE(-1); }

  zero_string(line, MAXLINE);
  for ( int lno = 0; ; lno++ ) { 
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    if ( strlen(line) == 0 ) {
      fprintf(stderr, "Error on line %d = [%s] \n", lno, line);
      go_BYE(-1);
    }
    switch ( ifldtype ) { 
    case I_INT : 
      itemp = strtol(line, &endptr, 10);
      if ( ( *endptr != '\0' ) && ( *endptr != '\n' ) )  {
	fprintf(stderr, "Error on line %d = [%s] \n", lno, line);
	go_BYE(-1);
      }
      fwrite(&itemp, 1, sizeof(int), ofp);
      break;
    case I_LONG_LONG : 
      ltemp = strtoll(line, &endptr, 10);
      if ( ( *endptr != '\0' ) && ( *endptr != '\n' ) )  {
	fprintf(stderr, "Error on line %d = [%s] \n", lno, line);
	go_BYE(-1);
      }
      fwrite(&ltemp, 1, sizeof(long long), ofp);
      break;
    default : 
      go_BYE(-1);
      break;
    }
    zero_string_to_nullc(line);
  }
 BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  return(status);
}
