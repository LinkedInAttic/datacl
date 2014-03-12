/* 

Input:  A (usually LaTeX file)  
Output: B  (usually LaTeX file). Created newly.

Reads a line from A at a time and writes it to B.
However, if it comes across a string of the form [[foo]],
then it replaces the string with the contents of the file foo
If no such file is found, it errors out

Note that we require the open [[ and the closing ]] to be on the same
line.

Currently, we assume that there is zero or one square bracket pairs on a
line.  This should be relaxed.

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "auxil.h"
// #include "html_encode.h"

extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);

#define BUFSIZE 65536
#define MAX_VAL_LEN 1024

// START FUNC DECL
int
txt_sub(
    char *infile,
    char *outfile
     )
// STOP FUNC DECL
{
  int status = 0;
  char *buffer = NULL;
  char filename[MAX_VAL_LEN];
  size_t bufsize = BUFSIZE;
  char *cptr = NULL, *cptr1 = NULL, *cptr2 = NULL;
  FILE *ifp = NULL, *ofp = NULL;
  char *X = NULL; size_t nX = 0;
  int i, nr, lno;

  if ( *infile == '\0' ) { go_BYE(-1); }
  if ( *outfile == '\0' ) { go_BYE(-1); }
  if ( strcmp(infile, outfile)== 0 ) { go_BYE(-1); }

  buffer = (char *)malloc(bufsize * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(buffer, bufsize);
  zero_string(filename, MAX_VAL_LEN);

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(outfile, "w");
  return_if_fopen_failed(ofp, outfile, "w");

  for ( lno = 1; ; lno++ ) {
    zero_string_to_nullc(buffer);
    zero_string_to_nullc(filename);
    nr = getline(&buffer, &bufsize, ifp);
    if ( nr <= 0 ) { break; } 
    cptr1 = cptr2 = NULL;
    cptr1 = strstr(buffer, "[["); 
    if ( cptr1 == NULL ) { 
      fprintf(ofp, "%s", buffer);
    }
    else {
      cptr2 = strstr(cptr1, "]]"); 
      if ( cptr2 == NULL ) { 
	fprintf(stderr, "No matching close bracket on Line %d = %s \n",
	    lno, buffer);
	go_BYE(-1);
      }
      for ( i = 0, cptr = cptr1 + strlen("[["); cptr < cptr2; cptr++, i++ ) { 
	filename[i] = *cptr;
      }
      /* Print part of line before square brackets */
      for ( cptr = buffer; cptr < cptr1; cptr++ ) {
	fprintf(ofp, "%c", *cptr);
      }
      /* Print contents of file */
      status = rs_mmap(filename, &X, &nX, 1);
      cBYE(status);
      for ( i = 0; i < nX; i++ )  {
	fprintf(ofp, "%c", X[i]);
      }
      /* Print part of line after square brackets */
      for ( cptr = cptr2 + strlen("]]"); *cptr != '\0'; cptr++ )  {
	fprintf(ofp, "%c", *cptr);
      }
      rs_munmap(X, nX);
    }
  }
 BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  free_if_non_null(buffer);
  return status ;
}
