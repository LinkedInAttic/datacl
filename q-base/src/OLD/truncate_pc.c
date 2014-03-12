/* This is a custom utility used to truncate US zip codes to 5 digits */
#include <stdio.h> 
#include <limits.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"

#define MAXLINE 256

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile = NULL, *opfile = NULL;
  char line[MAXLINE]; char *cptr = NULL;
  FILE *ifp = NULL, *ofp = NULL;

  if ( argc != 3 ) { go_BYE(-1); }
  infile = argv[1];
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");

  opfile = argv[2];
  ofp = fopen(opfile, "w");
  return_if_fopen_failed(ofp, opfile, "w");

  zero_string(line, MAXLINE);
  for ( int lno = 1; !feof(ifp); lno++ ) {
    int len;
    fgets(line, MAXLINE, ifp);
    if ( *line == '\0' ) { break; }
    len = strlen(line);
    if ( line[len-1] != '\n' ) { 
      fprintf(stderr, "Error on Line %d \n", lno); go_BYE(-1); 
    }
    if ( line[len-2] != '"' ) { 
      fprintf(stderr, "Error on Line %d \n", lno); go_BYE(-1); 
    }
    line[len-1] = '\0'; // delete eoln
    line[len-2] = '\0'; // delete last dquote
    cptr = line + 1; // delete first dquote
    len -= 3; /* length of line after quotes removed  */
    if ( *cptr == '\0' ) { 
      fprintf(ofp, "\"\"\n");
    }
    else {
      fprintf(ofp, "\"");
      for ( int i = 0; ( ( i < len ) && ( i < 5 ) ) ; i++ ) {
        fprintf(ofp, "%c", *cptr++);
      }
      fprintf(ofp, "\"\n");
    }
    zero_string_to_nullc(line);
  }
BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  return(status);
}
