#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include "macros.h"

// START FUNC DECL
void
zero_string(
    char *X,
    const int nX
    )
// STOP FUNC DECL
{
  for ( int i = 0; i < nX; i++ ) { X[i] = '\0'; }
}


// START FUNC DECL
void
zero_string_to_nullc(
    char *X
    )
// STOP FUNC DECL
{
  for ( ; *X != '\0'; X++ ) { 
    *X = '\0';
  }
}

int 
xcat(
      char *buf, 
      char *Y, 
      size_t nY, 
      size_t *ptr_iY
      )
{
    int status = 0;
    size_t iY = *ptr_iY;
    if ( ( buf == NULL ) || ( *buf == '\0' ) ) { go_BYE(-1); }
    int len = strlen(buf);
    if ( iY + len > nY ) { go_BYE(-1); }
    strncpy(Y+iY, buf, len);
    iY += len;
    *ptr_iY = iY;
BYE:
  return status ;
}

// START FUNC DECL
int
csv_to_json(
    char *X, 
    size_t nX, 
    int nR,
    char **ptr_Y, 
    size_t *ptr_nY
    )
// STOP FUNC DECL
{
  int status = 0;
  char *Y = NULL; size_t nY = 0;
  char buf[1024]; 
  char buf0[1024]; size_t n0 = 1024, i0 = 0;
  char buf1[1024]; size_t n1 = 1024, i1 = 0;

  if ( X == NULL ) { go_BYE(-1); }
  if ( nX == 0 ) { go_BYE(-1); }

  size_t lnX = 0;
  nY = 999999;
  Y = malloc(nY * sizeof(char));
  return_if_malloc_failed(Y);
  size_t iY = 0;
  status = xcat("{\n", Y, nY, &iY); cBYE(status);
  // figure out number of columns
  int nC = 1;
  char *cptr = X;
  for ( int i = 0; ( ( i < nX ) && ( *cptr != '\n' ) ) ; i++, cptr++ ) {
    if ( *cptr == ',' ) { 
      nC++;
    }
  }

  zero_string(buf0, 1024);
  zero_string(buf1, 1024);
  int col_idx = 0;
  int row_idx = 0;
  for ( row_idx = 0; row_idx < nR; row_idx++) { 
    sprintf(buf1, "    \"r%d\": {\n", row_idx);
    status = xcat(buf1, Y, nY, &iY); cBYE(status);
    for ( col_idx = 0; col_idx < nC; col_idx++) { 
      sprintf(buf1, "        \"c%d\": \"", col_idx);
      xcat(buf1, buf0, n0, &i0); cBYE(status);
      char *cptr = X + lnX;
      for ( ; ( ( *cptr != '\n' ) && ( *cptr != ',' ) ) ; cptr++ ) {
        buf0[i0++] = X[lnX++];
      }
      buf0[i0++] = '\"';
      if ( col_idx < (nC-1) ) { buf0[i0++] = ','; }
      buf0[i0++] = '\n';
      status = xcat(buf0, Y, nY, &iY); cBYE(status);
      lnX++; /* jump over delimiter */
      if ( lnX == nX ) { break; } // done with CSV file 
      zero_string_to_nullc(buf1); i1 = 0;
      zero_string_to_nullc(buf0); i0 = 0;
    }
    if ( row_idx < (nR-1) ) { 
      status = xcat("    },\n", Y, nY, &iY); cBYE(status);
    }
    else {
      status = xcat("    }\n", Y, nY, &iY); cBYE(status);
    }
  }
  status = xcat("}\n", Y, nY, &iY); cBYE(status);
  if ( lnX != nX ) { go_BYE(-1); }

  *ptr_Y = Y;
  *ptr_nY = iY;
BYE:
  return status ;
}

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *Y = NULL; size_t nY = 0;

  char *X = "abc,def\nghi,jkl\nxxx,yyy\n";
  size_t nX = strlen(X);
  status = csv_to_json(X, nX, 3, &Y, &nY); cBYE(status);
  fprintf(stderr, "%ld\n", nY);
  fprintf(stderr, "%s\n", Y);
BYE:
  return status ;
}
