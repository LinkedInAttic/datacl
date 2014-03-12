#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "auxil.h"

/* Given a single column integer file, it converts it into binary.
   Inputs 
   (1) Input file
   (2) fldtype, can be int or long long
   Output
   (1) Output file

   * backslashes and dealing with eoln within the quotes */

#define MAXLINE 32
#define I4 1
#define I8 2
// START FUNC DECL
int
asc2bin(
    char *infile,
    char *fldtype,
    char *outfile
     )
// STOP FUNC DECL
{
  int status = 0;
  int ifldtype;
  FILE *ifp = NULL;
  FILE *ofp = NULL;
  char *cptr;
  char line[MAXLINE];
  int tempI4; long long tempI8;

  if ( strcmp(infile, outfile) == 0 ) { go_BYE(-1); }

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(outfile, "wb");
  return_if_fopen_failed(ofp, outfile, "wb");
  if ( strcasecmp(fldtype, "I4") == 0 ) {
    ifldtype = I4;
  }
  else if ( strcasecmp(fldtype, "I8") == 0 ) {
    ifldtype = I8;
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
    case I4 : 
      status = stoI4(line, &tempI4); cBYE(status);
      fwrite(&tempI4, 1, sizeof(int), ofp);
      break;
    case I8 : 
      status = stoI8(line, &tempI8); cBYE(status);
      fwrite(&tempI8, 1, sizeof(long long), ofp);
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
  return status ;
}
