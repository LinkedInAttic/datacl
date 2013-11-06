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
#include "constants.h"
#include "macros.h"
#define BSEARCH_LOWEST 1 
#define BSEARCH_HIGHEST 2
#define BSEARCH_DONTCARE 3
/*-------------------------------------------------------------------*/
// START FUNC DECL
int
bin_search_UI8(
	   const unsigned long long *X,
	   long long nX,
	   unsigned long long key,
	   long long *ptr_pos,
	   char *str_direction
	   )
// STOP FUNC DECL
{
  int status = 0;
  long long pos, lb, ub, mid;
  int direction;

  if ( X == NULL ) go_BYE(-1);
  if ( nX <= 0 ) go_BYE(-1);
  if ( nX == 1 ) { 
    if ( X[0] == key ) { 
      *ptr_pos = 0; 
    }
    else {
      *ptr_pos = -1;
    }
    return(status); 
  }

  lb = 0; ub = nX - 1;
  *ptr_pos = pos = -1;
  for ( ; lb <= ub ; ) { 
    mid = ( lb + ub )  / 2 ;
    if ( X[mid] == key ) { 
      pos = mid;
      break;
    }
    else if ( key < X[mid] ) { /* reduce ub */
      ub = min(ub - 1, mid);
    }
    else /* key > X[mid] */ { /* increase lb */
      lb = max(lb + 1, mid);
    }
  }
  /* The following is meant to handle the case when the values are not unique
   */
  if ( ( str_direction == NULL ) || ( strlen(str_direction) == 0 ) ) { 
    direction = BSEARCH_DONTCARE; // Default setting
  }
  else if ( strcasecmp(str_direction, "lowest") == 0  ) {
    direction = BSEARCH_LOWEST; 
  }
  else if ( strcasecmp(str_direction, "highest") == 0  ) {
    direction = BSEARCH_HIGHEST; 
  }
  else if ( strcasecmp(str_direction, "dontcare") == 0  ) {
    direction = BSEARCH_DONTCARE; 
  }
  else { 
    go_BYE(-1);
  }
  switch ( direction ) { 
  case BSEARCH_LOWEST : 
    for ( ; pos > 0; ) { 
      if ( X[pos-1] == key ) { pos--; } else { break; }
    }
    break;
  case BSEARCH_HIGHEST : 
    for ( ; pos < nX-1; ) { 
      if ( X[pos+1] == key ) { pos++; } else { break; }
    }
    break;
  case BSEARCH_DONTCARE : 
    /* Nothing to do */
    break;
  default : go_BYE(-1);
  }
  *ptr_pos = pos;
 BYE:
  return(status);
}
