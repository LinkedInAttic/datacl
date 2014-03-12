#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
// START FUNC DECL
int
replace_char(
    char *infile,
    char *str_inchar,
    char *str_outchar
    )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  char inchar, outchar;

  status = rs_mmap(infile, &X, &nX, 1); cBYE(status);
  if ( nX == 0 ) { go_BYE(-1); }
  //-----------------------------------------------------
  if ( strlen(str_inchar) > 1 ) {
    if ( strcmp(str_inchar, "bslashn") == 0 ) {
      inchar = '\n';
    }
    else if ( strcmp(str_inchar, "bslashr") == 0 ) {
      inchar = '\r';
    }
    else { go_BYE(-1); }
  }
  else {
    inchar = *str_inchar;
  }
  //-----------------------------------------------------
  if ( strlen(str_outchar) > 1 ) {
    if ( strcmp(str_outchar, "bslashn") == 0 ) {
      outchar = '\n';
    }
    else if ( strcmp(str_outchar, "bslashr") == 0 ) {
      outchar = '\r';
    }
    else { go_BYE(-1); }
  }
  else {
    outchar = *str_outchar;
  }
  //-----------------------------------------------------
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] == inchar ) { X[i] = outchar; }
  }
BYE:
   rs_munmap(X, nX);
  return(status);
}
