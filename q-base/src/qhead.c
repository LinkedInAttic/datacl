/* Processes input file a line at a time. If the line number is between
 * the bounds specified, it echoes it. Use null string if you do not
 * wish to specify upper or lower bounds. Both bounds are inclusive
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "qhead.h"
#include "auxil.h"

#define MAXLINE   65536
// START FUNC DECL
int
qhead(
    char *infile,
    char *str_lb_incl,
    char *str_ub_incl
    )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL;
  int first_line = 0;
  int last_line = 0;
  char line[MAXLINE];
  int lno; 

  zero_string(line, MAXLINE);
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  if ( strlen(str_lb_incl) == 0 ) { 
    first_line = INT_MIN;
  }
  else {
    status = stoI4(str_lb_incl, &first_line);  cBYE(status);
  }
  if ( strlen(str_ub_incl) == 0 ) { 
    last_line = INT_MAX;
  }
  else {
    status = stoI4(str_ub_incl, &last_line);  cBYE(status);
  }
  if ( first_line > last_line ) { go_BYE(-1); }
  for ( lno = 0; ; lno++ ) {
    fgets(line, MAXLINE, ifp);
    if ( *line == '\0' ) { break; }
    if ( ( lno >= first_line ) && ( lno <= last_line ) ) { 
      fprintf(stdout, "%s", line);
    }
    zero_string_to_nullc(line);
  }
BYE:
  return(status);
}
