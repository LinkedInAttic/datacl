#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *opfile = NULL, *fldtype = NULL;
  FILE *ofp = NULL;
  int nR, nC, ctr, itemp; long long ltemp;
  char *endptr, *cptr;
  char *strN ;
  wchar_t wc;

  if ( argc != 4 ) { go_BYE(-1); }
  fldtype = argv[1];
  strN    = argv[2];
  opfile  = argv[3];
  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp, opfile, "wb");
  if ( strcmp(fldtype, "int") == 0 ) { 
    nR = strtol(strN, &endptr, 10);
    if ( nR <= 0 ) { go_BYE(-1); }
    for ( int i = 0; i < nR; i++ ) { 
      itemp = i+1;
      fwrite(&itemp, sizeof(int), 1, ofp);
    }
  }
  else if ( strcmp(fldtype, "long long") == 0 ) { 
    nR = strtol(strN, &endptr, 10);
    if ( nR <= 0 ) { go_BYE(-1); }
    for ( int i = 0; i < nR; i++ ) { 
      ltemp = i+1;
      fwrite(&ltemp, sizeof(long long), 1, ofp);
    }
  }
  else if ( strcmp(fldtype, "wchar") == 0 ) { 
    cptr = strsep(&strN, ":");
    if ( cptr == NULL ) { go_BYE(-1); }
    nR = strtol(cptr, &endptr, 10);
    cptr = strsep(&strN, ":");
    if ( cptr == NULL ) { go_BYE(-1); }
    nC = strtol(cptr, &endptr, 10);
    ctr = 32;
    for ( int i = 0; i < nR; i++ ) { 
      for ( int j = 0; j < nC; j++ ) { 
	wc = (wchar_t)ctr;
        fwrite(&wc, sizeof(wchar_t), 1, ofp);
	ctr++;
	if ( ctr == 127 ) { ctr = 32; }
      }
      wc = (wchar_t)0;
      fwrite(&wc, sizeof(wchar_t), 1, ofp);
    }
  }
  else { 
    go_BYE(-1);
  }
BYE:
  fclose_if_non_null(ofp);
  return(status);
}
