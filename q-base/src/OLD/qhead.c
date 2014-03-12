#include <stdio.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
/* Processes input file a line at a time. If the line number is between
 * the bounds specified, it echoes it. Use null string if you do not
 * wish to specify upper or lower bounds. Both bounds are inclusive
 */

#define MAXLINE   65536
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  FILE *ifp = NULL;
  int first_line = 0;
  int last_line = 0;
  char line[MAXLINE];
  int lno; char *endptr;

  zero_string(line, MAXLINE);
  if ( argc != 4 ) { go_BYE(-1); }
  ifp = fopen(argv[1], "r");
  return_if_fopen_failed(ifp, argv[1], "r");
  if ( strlen(argv[2]) == 0 ) { 
    first_line = INT_MIN;
  }
  else {
    first_line = strtol(argv[2], &endptr, 10);
  }
  if ( strlen(argv[3]) == 0 ) { 
    first_line = INT_MAX;
  }
  else {
    last_line  = strtol(argv[3], &endptr, 10);
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
