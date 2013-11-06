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
#include <string.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "find_closest_I4.h"

#define  LOW  100
#define  HIGH 200
// START FUNC DECL
int
find_closest_I4(
    int *X, /* [n] */
    long long n, 
    int val,
    char *str_mode, /* highest or lowest index */
    long long *ptr_idx
    )
// STOP FUNC DECL
{
  int status = 0;
  int mode;
  if ( strcmp(str_mode, "lowest") == 0 ) {
    mode = LOW;
  }
  else if ( strcmp(str_mode, "highest") == 0 ) {
    mode = HIGH;
  }
  else {
    go_BYE(-1);
  }
  long long mid = LLONG_MIN;
  long long bot = 0;
  long long top = n-1;
  for ( ; ; ) { 
    mid = ( bot + top ) / 2;
    if ( ( mid == 0 ) || ( mid == n-1 ) ) {  
      *ptr_idx = mid;
      break;
    }
    if ( X[mid] > val ) { /* we need to look in bottom half */
      top = mid;
    }
    else if ( X[mid] < val ) { /* we need to look in bottom half */
      bot = mid;
    }
    else {
      if ( mode == LOW ) {
	if ( X[mid-1] == val ) {
	  top = mid;
	}
	else {
	  *ptr_idx = mid; break;
	}
      }
      else if ( mode == HIGH ) {
	if ( X[mid+1] == val ) {
	  bot = mid;
	}
	else {
	  *ptr_idx = mid; break;
	}
      }
      else {
	go_BYE(-1);
      }
    }
  }
BYE:
  return(status);
}

#undef UNIT_TEST
#ifdef UNIT_TEST
int
main()
{
  int status = 0;
#define N 33
  int X[N];
  long long idx;

  for ( int   i = 0; i < N; i++ ) { 
    X[i] = 2*i;
  }
  status = find_closest_I4(X, (long long)N, 3, "lowest", &idx); cBYE(status);
  fprintf(stderr, "idx = %lld \n", idx);
BYE:
  return(status);
}


#endif
