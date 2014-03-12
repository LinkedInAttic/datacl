#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
// START FUNC DECL
int
extract_S(
    const char *X,
    const char *start_str,
    const char *stop_str,
    char *output,
    int len_output,
    bool *ptr_is_null
    )
// STOP FUNC DECL
{
  int status = 0;
  int i;
  *ptr_is_null = true;
  if ( X == NULL ) { *ptr_is_null = true; return status ; }
  char *Y = strstr(X, start_str);
  if ( Y == NULL ) {  *ptr_is_null = true; return status ; }
  char *Z = Y + strlen(start_str);
  char *W = strstr(Z, stop_str);
  if ( W == NULL ) {  *ptr_is_null = true; return status ; }
  if ( ( W - Z ) >= (len_output-1) ) { go_BYE(-1); } // string too long 
  for ( i = 0; Z != W ; ) {
    output[i++] = *Z++;
  }
  output[i] = '\0';
  *ptr_is_null = false;
BYE:
  return status ;
}

// START FUNC DECL
int
extract_F4(
    const char *X,
    const char *start_str,
    const char *stop_str,
    float *ptr_fval,
    bool *ptr_is_null
    )
// STOP FUNC DECL
{
  int status = 0;
#define BUFLEN 32
  char bufstr[BUFLEN];
  double dval;
  char *endptr;

  zero_string(bufstr, BUFLEN);
  status = extract_S(X, start_str, stop_str, bufstr, BUFLEN, ptr_is_null);
  cBYE(status);
  if ( *ptr_is_null ) { return status ; }
  dval = strtod(bufstr, &endptr);
  if ( *endptr != '\0' ) { *ptr_is_null = true; return status ; }
  if ( ( dval < FLT_MIN ) || ( dval > FLT_MAX ) ) { go_BYE(-1); }
  *ptr_fval = dval;
BYE:
  return status ;
}
