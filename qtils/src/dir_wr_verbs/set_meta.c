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
#include "set_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
int_set_meta(
	     int tbl_id,
	     int fld_id,
	     char *attr,
	     char *value
	     )
// STOP FUNC DECL
{
  int status = 0;
  FLD_TYPE fldtype;

  if ( attr == NULL ) { go_BYE(-1); }
  if ( value == NULL ) { go_BYE(-1); }
  if ( *value == '\0' ) { go_BYE(-1); }
  if ( strcmp(attr, "dict_tbl_id") == 0 ) {
    char *endptr;
    int dict_tbl_id = strtoll(value, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( ( g_flds[fld_id].fldtype != I1 )  &&
         ( g_flds[fld_id].fldtype != I2 )  &&
         ( g_flds[fld_id].fldtype != I4 ) ) { 
      go_BYE(-1); 
    }
    if ( dict_tbl_id > g_n_tbl ) { go_BYE(-1); }
    g_flds[fld_id].dict_tbl_id = dict_tbl_id;
  }
  else if ( strcmp(attr, "cnt") == 0 ) {
    char *endptr;
    long long cnt = strtoll(value, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( cnt > g_tbls[g_flds[fld_id].tbl_id].nR ) { go_BYE(-1); }
    if ( cnt < 0 ) { if ( cnt != -1 ) { go_BYE(-1); } }
    g_flds[fld_id].cnt = cnt;
  }
  else if ( strcmp(attr, "fldtype") == 0 ) {
    status = unstr_fldtype(value, &fldtype); cBYE(status);
    if ( g_flds[fld_id].fldtype == fldtype ) { 
      /* Nothing to do */
      goto BYE;
    }
    g_flds[fld_id].fldtype = fldtype;
  }
  else if ( strcmp(attr, "sort_type") == 0 ) {
    if ( strcmp(value, "unknown") == 0 ) { 
      g_flds[fld_id].sort_type = unknown; 
    }
    else if ( strcmp(value, "ascending") == 0 ) { 
      g_flds[fld_id].sort_type = ascending; 
    }
    else if ( strcmp(value, "descending") == 0 ) { 
      g_flds[fld_id].sort_type = descending; 
    }
    else if ( strcmp(value, "unsorted") == 0 ) { 
      g_flds[fld_id].sort_type = unsorted; 
    }
    else { 
      go_BYE(-1);
    }
  }
  else if ( strcmp(attr, "is_dict_tbl") == 0 ) {
    if ( fld_id >= 0 ) { go_BYE(-1); }
    if ( strcmp(value, "true") == 0 ) {
      g_tbls[tbl_id].is_dict_tbl = true;
    }
    else if ( strcmp(value, "false") == 0 ) {
      g_tbls[tbl_id].is_dict_tbl = false;
    }
    else { go_BYE(-1); }
  }
  else if ( strcmp(attr, "disp_name") == 0 ) {
    if ( fld_id < 0 ) { // setdisp name for tbl
      if ( strlen(value) > MAX_LEN_TBL_DISP_NAME ) { go_BYE(-1); }
      for ( int i = 0; i < g_n_tbl; i++ ) { 
	if ( strcmp(g_tbls[i].dispname, value) == 0 ) { 
	  fprintf(stderr, "This display name is in use\n");
	  go_BYE(-1);
	}
      }
      strcpy(g_tbls[tbl_id].dispname, value);
    }
    else { 
      if ( strlen(value) > MAX_LEN_FLD_DISP_NAME ) { go_BYE(-1); }
      for ( int i = 0; i < g_n_fld; i++ ) { 
	if ( ( g_flds[i].tbl_id == tbl_id ) && 
	     ( strcmp(g_flds[i].dispname, value) == 0 ) ) { 
	  fprintf(stderr, "This display name is in use\n");
	  go_BYE(-1);
	}
      }
      strcpy(g_flds[fld_id].dispname, value);
    }
  }
  else {
    fprintf(stderr, "NOT IMPLEMENTED. attr = [%s] \n", attr);
    go_BYE(-1);
  }
 BYE:
  return(status);
}
int
set_meta(
	 char *tbl,
	 char *fld,
	 char *attr,
	 char *value
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
  status = int_set_meta(tbl_id, fld_id, attr, value); cBYE(status);
 BYE:
  return(status);
}
