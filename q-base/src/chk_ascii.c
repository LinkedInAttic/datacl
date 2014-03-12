/* 
   Reads a file, a line at a time. If there is a non-ascii character, it
   prints the line number. Ideally, once it has seen a particular bad
   character, future lines should not be tagged as bad by the presence
   of that bad character. Some other bad character is needed to flag it
   as bad
   */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"

#define BUFLEN 65536
// START FUNC DECL
int
chk_ascii(
    char *infile
    )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL;
  char buffer[BUFLEN];
  char *cptr = NULL;
  char bad_chars[1024]; int n_bad_chars = 0;
  bool seen_before = false;

  zero_string(buffer, BUFLEN);
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( int lno = 1; !feof(ifp); lno++ ) { 
    cptr = fgets(buffer, BUFLEN-1, ifp);
    if ( cptr == NULL ) { break; }
    for ( int i = 0; i < BUFLEN; i++ ) { 
      if ( buffer[i] == '\0' ) { break; }
      if ( !isascii(buffer[i]) ) {
	seen_before = false;
	for ( int j = 0; j < n_bad_chars; j++ ) { 
	  if ( buffer[i] == bad_chars[j] ) { 
	    seen_before = true;
	    break;
	  }
	}
	if ( !seen_before ) { 
	  // Add to bad chars 
	  bad_chars[n_bad_chars++] = buffer[i];
	  // Print line 
	  fprintf(stderr, "Line %5d: Bad character = [%c] \n", lno, buffer[i]); 
	  break;
	}
      }
    }
    zero_string_to_nullc(buffer);
  }
  fclose_if_non_null(ifp);
BYE:
  fclose_if_non_null(ifp);
  return status ;
}
