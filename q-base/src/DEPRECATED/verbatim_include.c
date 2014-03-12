/* 

Input:  A (usually LaTeX file)  
Output: B  (usually LaTeX file). Created newly.

Reads a line from A at a time and writes it to B.
However, if it comes across a string of the form 
%% INCLUDE <infile> <start_label> <stop_label> 
then 
(1) Create a temporary file from the infile, starting just after the line 
#start_label or //start_label
and stopping just before the line 
#stop_label or //stop_label
(2) If no labels are provided, then the entire file is included
(3) If only one label is provided, then it is considered to be the
start_label and we copy until eof

Note that A cannot be equal to B

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "open_temp_file.h"

extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);

bool g_write_to_temp_dir = false;

#define BUFSIZE 65536
#define MAX_VAL_LEN 1024

// START FUNC DECL
int
verbatim_include(
    char *infile,
    char *outfile
    )
// STOP FUNC DECL
{
  int status = 0;
  char *buffer = NULL;
  char filename[256];
  size_t bufsize = BUFSIZE;
  FILE *ifp = NULL, *ofp = NULL, *tfp = NULL;
  char *tempfile = NULL;
  int nr, lno;

  if ( *infile == '\0' ) { go_BYE(-1); }
  if ( *outfile == '\0' ) { go_BYE(-1); }
  if ( strcmp(infile, outfile)== 0 ) { go_BYE(-1); }

  buffer = (char *)malloc(bufsize * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(buffer, bufsize);
  zero_string(filename, 256);

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(outfile, "w");
  return_if_fopen_failed(ofp, outfile, "w");

  for ( lno = 1; ; lno++ ) {
    zero_string_to_nullc(buffer);
    nr = getline(&buffer, &bufsize, ifp);
    if ( nr <= 0 ) { break; } 
    if ( ( strcasecmp(buffer, "%% INCLUDE") == 0 ) || 
         ( strcasecmp(buffer, "// INCLUDE") == 0 ) ) {
      status = open_temp_file(&tfp, &tempfile); cBYE(status);
      fclose_if_non_null(tfp);
      fprintf(ofp, "\\verbatiminput{%s}\n", tempfile);
      free_if_non_null(tempfile);
    }
    else { // copy from input to output 
      fprintf(ofp, "%s", buffer); 
     }
  }
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
 BYE:
  free_if_non_null(buffer);
  return status ;
}
