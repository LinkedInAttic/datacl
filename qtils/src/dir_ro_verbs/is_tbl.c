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
#include <limits.h>
#include "qtypes.h"
#include "hash_string.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "meta_globals.h"

extern char *g_docroot;
// START FUNC DECL
int
is_tbl(
    const char *tbl,
    int *ptr_tbl_id, 
    TBL_REC_TYPE *ptr_tbl_rec
    )
// STOP FUNC DECL
{
  int status = 0;
  int idx, startloc; unsigned int hashval;

  *ptr_tbl_id = INT_MIN;
  zero_tbl_rec(ptr_tbl_rec);
  status = hash_string_UI4(tbl, &hashval); cBYE(status);
  idx = hashval % g_n_ht_tbl;
  startloc = g_ht_tbl[idx].val;
  if ( startloc < 0 ) { startloc = 0; }
    
  for ( int i = startloc; i < g_n_tbl; i++ ) { 
    if ( strcmp(g_tbls[i].name, tbl) == 0 ) {
      *ptr_tbl_id = i;
      *ptr_tbl_rec = g_tbls[i];
      break;
    }
  }
  if ( *ptr_tbl_id < 0 ) { /* not found in above loop */
    for ( int i = 0; i < startloc; i++ ) { 
      if ( strcmp(g_tbls[i].name, tbl) == 0 ) {
	*ptr_tbl_id = i;
        *ptr_tbl_rec = g_tbls[i];
	break;
      }
    }
  }
BYE:
  return(status);
}

