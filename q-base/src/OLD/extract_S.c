#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
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
  if ( X == NULL ) { *ptr_is_null = true; return(status); }
  char *Y = strstr(X, start_str);
  if ( Y == NULL ) {  *ptr_is_null = true; return(status); }
  char *Z = Y + strlen(start_str);
  char *W = strstr(Z, stop_str);
  if ( W == NULL ) {  *ptr_is_null = true; return(status); }
  if ( ( W - Z ) >= (len_output-1) ) { go_BYE(-1); } // string too long to be integer
  for ( i = 0; Z != W ; ) {
    output[i++] = *Z++;
  }
  output[i] = '\0';
  *ptr_is_null = false;
BYE:
  return(status);
}
