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
#include "is_tbl.h"
#include "is_fld.h"
#include "mmap.h"
#include "aux_meta.h"
#include "get_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
int_get_meta(
	     int tbl_id,
	     int fld_id,
	     char *attr,
	     char *value
	     )
// STOP FUNC DECL
{
  int status = 0;

  chk_range(tbl_id, 0, g_n_tbl);
  chk_range(fld_id, 0, g_n_fld);
  if ( g_flds[fld_id].tbl_id != tbl_id ) { go_BYE(-1); }
  if ( attr == NULL ) { go_BYE(-1); }
  if ( value == NULL ) { go_BYE(-1); }
  if ( strcmp(attr, "sort_type") == 0 ) {
    switch ( g_flds[fld_id].sort_type ) { 
      case unknown : strcpy(value, "unknown"); break;
      case ascending : strcpy(value, "ascending"); break;
      case descending : strcpy(value, "descending"); break;
      case unsorted : strcpy(value, "unsorted"); break;
      default : go_BYE(-1); break;
    }
  }
  else if ( strcmp(attr, "is_external") == 0 ) {
    sprintf(value, "%d", g_flds[fld_id].is_external);
  }
  else if ( strcmp(attr, "dict_tbl_id") == 0 ) {
    sprintf(value, "%d", g_flds[fld_id].dict_tbl_id);
  }
  else { go_BYE(-1); }
BYE:
  return(status);
}
// START FUNC DECL
int
get_meta(
	 char *tbl,
	 char *fld,
	 char *attr,
	 char *rslt_buf,
	 int sz_rslt_buf // TODO P3: Use this to check for buffer overflow
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id;
  FLD_REC_TYPE fld_rec; int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status); 
  chk_range(tbl_id, 0, g_n_tbl); 
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { 
    fld_id = INT_MIN;
  }
  else { 
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec);
    cBYE(status); 
    chk_range(fld_id, 0, g_n_fld);
  }
  status = int_get_meta(tbl_id, fld_id, attr, rslt_buf); cBYE(status);
 BYE:
  return(status);
}
