#include <stdio.h>
#define __USE_XOPEN /* Necessary for strptime to be defined */
#include <time.h>
#include <stdbool.h>
// #include "/usr/include/time.h"
#include "constants.h"
#include "macros.h"
#include "fsize.h"
// START FUNC DECL
int
dateconv(
    char *in_X, /* input string */
    int *in_sz_X, /* size spec for input */
    long long nX, /* number of rows */
    char *date_format,
    int *in_output
    )
// STOP FUNC DECL
{
  int status = 0;
#define MAX_LEN_DATE 31
  char buffer[MAX_LEN_DATE+1];
  char *X = in_X;
  int *sz_X = in_sz_X;
  int *output = in_output;
  struct tm tm;
  int sz;

  zero_string(buffer, MAX_LEN_DATE+1);
  for ( long long i = 0; i < nX; i++ ) { 
    sz = *sz_X;
    if ( sz >= MAX_LEN_DATE ) { 
      fprintf(stderr, "String [%s] too long to be a date \n", X);
      go_BYE(-1);
    }
    if ( sz == 1 ) { /* null string */
      *output = 0;
    }
    else {
      strptime(X, date_format, &tm);
      *output = mktime(&tm);
    }
    // Move to next string 
    output++;
    sz_X++;
    X += sz;
  }

BYE:
  return status ;
}
