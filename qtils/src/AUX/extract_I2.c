/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
// START FUNC DECL
int
extract_I2(
    const char *X,
    const char *start_str,
    const char *stop_str,
    short *ptr_ival,
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
  *ptr_ival = (short)strtoll(buffer, &endptr, 10);
  if ( ( *ptr_ival < SHRT_MIN ) || ( *ptr_ival > SHRT_MAX ) ) {
    go_BYE(-1); 
  }
  if ( *endptr != '\0' ) { go_BYE(-1); }
  *ptr_is_null = false;
BYE:
  return(status);
}

