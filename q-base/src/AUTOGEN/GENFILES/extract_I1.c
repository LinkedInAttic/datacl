#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
extract_I1(
    const char *X,
    const char *start_str,
    const char *stop_str,
    char *ptr_ival,
    bool *ptr_is_null
    )
// STOP FUNC DECL
{
  int status = 0;
  int i;
#define MAX_LEN 32
  char buffer[MAX_LEN];
  *ptr_is_null = true;
  if ( X == NULL ) { *ptr_is_null = true; return 0; }
  char *Y = strstr(X, start_str);
  if ( Y == NULL ) {  *ptr_is_null = true; return 0; }
  char *Z = Y + strlen(start_str);
  char *W = strstr(Z, stop_str);
  if ( W == NULL ) {   *ptr_is_null = true; return 0; }
  if ( ( W - Z ) >= MAX_LEN ) { go_BYE(-1); } // string too long to be integer
  for ( i = 0; Z != W ; ) {
    buffer[i++] = *Z++;
  }
  buffer[i] = '\0';
  char *endptr;
  if ( buffer[0] == '\0' ) { *ptr_is_null = true; return 0; }
  *ptr_ival = (char)strtoll(buffer, &endptr, 10);
  if ( ( *ptr_ival < SCHAR_MIN ) || ( *ptr_ival > SCHAR_MAX ) ) {
    go_BYE(-1); 
  }
  if ( *endptr != '\0' ) { go_BYE(-1); }
  *ptr_is_null = false;
BYE:
  return(status);
}

