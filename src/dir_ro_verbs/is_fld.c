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
#include "is_tbl.h"
#include "aux_meta.h"
#include "is_fld.h"

#include "meta_globals.h"
extern char *g_docroot;
// START FUNC DECL
int
  is_fld(
      const char *tbl,
      int in_tbl_id,
      const char *fld,
     int *ptr_fld_id,
     FLD_REC_TYPE *ptr_fld_rec,
     int *ptr_nn_fld_id,
     FLD_REC_TYPE *ptr_nn_fld_rec
      )
// STOP FUNC DECL
{
  int status = 0;
  int idx, startloc, tbl_id, nn_fld_id;
  unsigned int hashval;
  TBL_REC_TYPE  tbl_rec;

  *ptr_fld_id = INT_MIN;
  zero_fld_rec(ptr_fld_rec);
  *ptr_nn_fld_id = INT_MIN;
  zero_fld_rec(ptr_nn_fld_rec);

  if ( in_tbl_id < 0 ) { 
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    if ( tbl_id < 0 ) { goto BYE; }
  }
  else {
    tbl_id = in_tbl_id;
  }
  status = hash_string_UI4(fld, &hashval); cBYE(status);
  idx = hashval % g_n_ht_fld;
  startloc = g_ht_fld[idx].val;
  if ( startloc < 0 ) { startloc = 0; }
    
  for ( int i = startloc; i < g_n_fld; i++ ) { 
    if ( g_flds[i].tbl_id != tbl_id ) { continue; }
    if ( strcmp(g_flds[i].name, fld) == 0 ) {
      *ptr_fld_id = i;
      *ptr_fld_rec = g_flds[i];
      nn_fld_id = g_flds[i].nn_fld_id;
      *ptr_nn_fld_id = nn_fld_id;
      if ( nn_fld_id >= 0 ) { 
        *ptr_nn_fld_rec = g_flds[nn_fld_id];
      }
      break;
    }
  }
  if ( *ptr_fld_id < 0 ) { /* not found in above loop */
    for ( int i = 0; i < startloc; i++ ) { 
      if ( g_flds[i].tbl_id != tbl_id ) { continue; }
      if ( strcmp(g_flds[i].name, fld) == 0 ) {
	*ptr_fld_id = i;
        *ptr_fld_rec = g_flds[i];
	nn_fld_id = g_flds[i].nn_fld_id;
	*ptr_nn_fld_id = nn_fld_id;
        if ( nn_fld_id >= 0 ) { 
          *ptr_nn_fld_rec = g_flds[nn_fld_id];
	}
	break;
      }
    }
  }
BYE:
  return(status);
}
