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
#include "dbauxil.h"
#include "del_fld.h"
#include "is_tbl.h"
#include "auxil.h"
#include "tbl_meta.h"
#include "aux_meta.h"
#include "meta_globals.h"
extern char *g_docroot;
// START FUNC DECL
int
  del_tbl(
      const char *tbl,
      int in_tbl_id
      )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  TBL_REC_TYPE tbl_rec;
  // Basic arg checks 
  if ( in_tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  }
  else {
    tbl_id = in_tbl_id;
  }
  if ( tbl_id < 0 ) { /* Nothing to do */
    goto BYE;
  }
  //------------------------------------------------------
  // Make sure that nobody else is using this table
  bool is_in_use = false;
  int alt_tbl_id = INT_MIN, alt_fld_id = INT_MIN;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].dict_tbl_id == tbl_id ) { 
      is_in_use = true; 
      alt_tbl_id = g_flds[i].tbl_id;
      alt_fld_id = i;
      break; 
    }
  }
  if ( is_in_use ) {
    fprintf(stderr, "In use by field %s of table %s \n", 
	g_flds[alt_fld_id].name, 
	g_tbls[alt_tbl_id].name);
    go_BYE(-1);
  }

  //------------------------------------------------------
  // Delete all the primary fields in this table (aux will happen)
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( ( g_flds[i].tbl_id == tbl_id ) && (g_flds[i].parent_id < 0 ) ) {
      status = del_fld( NULL, tbl_id, NULL, i,  true);
      cBYE(status);
    }
  }
  //------------------------------------------------
  // Delete table itself
  zero_tbl_rec(&(g_tbls[tbl_id]));
BYE:
  return(status);
}
// START FUNC DECL
int  
iter_del_tbl(
	const char *tbl
	)
// STOP FUNC DECL
{
  int status = 0;
  char **Y = NULL; int nY = 0; bool is_multiple = false;

  for ( int i = 0; ; i++ ) { 
    if ( tbl[i] == '\0' ) { break; }
    if ( tbl[i] == ':' ) { is_multiple = true; break; }
  }
  if ( is_multiple ) {
    status = break_str(tbl, ":", &Y, &nY); cBYE(status);
    for ( int i = 0; i < nY; i++ ) { 
      status = del_tbl(Y[i], -1); cBYE(status);
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  else {
    status = del_tbl(tbl, -1); cBYE(status);
  }
BYE:
  return(status);
}
