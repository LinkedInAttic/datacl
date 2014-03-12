#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"

int
gen_data(
    char *fldtype,
    char *strN,
    char *opfile
    )
{
  int status = 0;
  FILE *ofp = NULL;
  int nR, itemp; long long ltemp;
  char *endptr;

  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp, opfile, "wb");
  if ( strcmp(fldtype, "I4") == 0 ) { 
    nR = strtol(strN, &endptr, 10);
    if ( nR <= 0 ) { go_BYE(-1); }
    for ( int i = 0; i < nR; i++ ) { 
      itemp = i+1;
      fwrite(&itemp, sizeof(int), 1, ofp);
    }
  }
  else if ( strcmp(fldtype, "I8") == 0 ) { 
    nR = strtol(strN, &endptr, 10);
    if ( nR <= 0 ) { go_BYE(-1); }
    for ( int i = 0; i < nR; i++ ) { 
      ltemp = i+1;
      fwrite(&ltemp, sizeof(long long), 1, ofp);
    }
  }
  else { 
    go_BYE(-1);
  }
BYE:
  fclose_if_non_null(ofp);
  return(status);
}
