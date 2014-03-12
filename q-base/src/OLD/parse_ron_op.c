#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
/* Ron's title mapper creates output of the form 

"engineeer and attorney"	engineer,attorney	1
doktor	medical doctor	1
junk		1
patent attorny		1
md	managing director	1

We want to map this to something like
0,engineer
0,attorney
1,medical doctor
2,""
3,""
4,"managing director"

......

*/

#define MAXLINE 65536
#define MAX_LEN_CLEAN_TITLE 128

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile = NULL, *opfile = NULL;
  FILE *ifp = NULL, *ofp = NULL;
  char line[MAXLINE];
  char pkbuf[16];
  char buffer[MAX_LEN_CLEAN_TITLE];
  char clean_titles[MAXLINE];
  char *cptr = NULL, *endptr;
  long long llpk;

  zero_string(pkbuf, 16);
  zero_string(line, MAXLINE);
  zero_string(clean_titles, MAXLINE);
  zero_string(buffer, MAX_LEN_CLEAN_TITLE);
  if ( argc != 3 ) { go_BYE(-1); }
  infile = argv[1];
  opfile = argv[2];
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  ofp = fopen(opfile, "w");
  return_if_fopen_failed(ofp, opfile, "w");
  for ( int lno = 0; ; lno++ ) { 
    zero_string(pkbuf, 16);
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    // First column contains the pk
    for ( int i = 0; *cptr != '\t' ; cptr++, i++ ) { 
      if ( ( *cptr == '\n' ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
      pkbuf[i] = *cptr;
    }
    llpk = strtoll(pkbuf, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( llpk < 0 ) { go_BYE(-1); }
    cptr++; // advance over the tab

    // skip over second tab
    for ( int i = 0; *cptr != '\t' ; cptr++, i++ ) { 
      if ( ( *cptr == '\n' ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    }
    cptr++; // advance over the tab

    // read till third tab
    for ( int i = 0; *cptr != '\t' ; i++, cptr++ ) { 
      if ( ( *cptr == '\n' ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
      clean_titles[i] = *cptr;
    }
    if ( *clean_titles == '\0' ) { 
      fprintf(ofp, "%d,\"\"\n", lno);
    }
    else {
      for ( cptr = clean_titles; ; ) { 
	for ( int i = 0; 
	    ( ( *cptr != ',' ) && ( *cptr != '\t' ) &&
	      ( *cptr != '\n' ) && ( *cptr != '\0' ) ); i++, cptr++ ) { 
	  buffer[i] = *cptr;
	}
	// Print only if buffer is not null
	if ( *buffer != '\0' ) {
          fprintf(ofp, "%s,%s\n", pkbuf, buffer);
	}
	zero_string_to_nullc(buffer);
	if ( *cptr == ',' ) {
	  cptr++;
	}
	else {
	  break;
	}
      }
    }
    zero_string_to_nullc(line);
    zero_string_to_nullc(clean_titles);
  }
BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  return(status);
}

