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
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "del_fld.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int  
del_fld(
	const char *tbl,
	int tbl_id,
	const char *fld,
	int fld_id,
	bool is_external_call
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  int nn_fld_id = -1;

  if ( tbl_id < 0 ) {
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  }
  if ( tbl_id < 0 ) { /* nothing to do */ goto BYE; }
  //------------------------------------------------------
  if ( fld_id < 0 ) { 
    if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
    status = is_fld(tbl, tbl_id, fld, &fld_id, &fld_rec, 
		    &nn_fld_id, &nn_fld_rec); 
    cBYE(status);
  }
  else {
    nn_fld_id = g_flds[fld_id].nn_fld_id;
    fld_rec = g_flds[fld_id];
    if ( nn_fld_id >= 0 ) { 
      nn_fld_rec = g_flds[nn_fld_id];
    }
    else {
      zero_fld_rec(&nn_fld_rec);
    }
  }
  if ( fld_id < 0 ) {  /* Nothing to do */ return(status); }
  //--------------------------------------------
  if ( is_external_call ) { 
    // You should not be deleting auxiliary fields directly
    if ( g_flds[fld_id].auxtype != undef ) { /* auxiliary field */
      int parent_id = g_flds[fld_id].parent_id;
      if ( g_flds[parent_id].name[0] == '\0' ) {
        fprintf(stderr, "Control should not come here \n");
      }
      else {
        fprintf(stderr, "Field [%s] in Table [%s] is nn field of %s.\n", 
		fld, tbl, g_flds[parent_id].name);
        fprintf(stderr, "Delete primary field to delete this field \n");
        go_BYE(-1);
      }
    }
  }
  //------------------------------------------------
  /* Unlink storage. Exceptions (1) external or (2) alias */
  if ( ( g_flds[fld_id].is_external == false ) && 
       ( g_flds[fld_id].alias_of_fld_id < 0 ) ) {
    int ddir_id = g_flds[fld_id].ddir_id;
    if ( ddir_id < 0 ) { 
      chdir(g_data_dir); 
    }
    else {
      char *alt_data_dir = g_ddirs[ddir_id].name;
      if (( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) {go_BYE(-1);}
      chdir(alt_data_dir); 
    }
    unlink(g_flds[fld_id].filename);
    chdir(g_cwd);
  }
  zero_fld_rec(&(g_flds[fld_id])); /* Delete entry for this field  */

  /* Delete aliases if any. TODO P3 Speed this up */
  if ( g_flds[fld_id].alias_of_fld_id < 0 ) {
    for ( int i = 0; i < g_n_fld; i++ ) { 
      if ( g_flds[fld_id].alias_of_fld_id == fld_id ) {
	zero_fld_rec(&(g_flds[fld_id])); /* Delete entry for this field  */
      }
    }
  }

  // Delete nn field if necessary
  if ( nn_fld_id >= 0 ) {
    if ( g_flds[nn_fld_id].is_external == false ) { 
      int nn_ddir_id = g_flds[nn_fld_id].ddir_id;
      if ( nn_ddir_id < 0 ) { 
	chdir(g_data_dir); 
      }
      else {
	char *alt_data_dir = g_ddirs[nn_ddir_id].name;
	if (( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' )) {go_BYE(-1);}
	chdir(alt_data_dir); 
      }
      unlink(g_flds[nn_fld_id].filename);
      chdir(g_cwd);
    }
    zero_fld_rec(&(g_flds[nn_fld_id])); /* Delete entry for this field  */
  }
 BYE:
  return(status);
}
// START FUNC DECL
int  
iter_del_fld(
	const char *tbl,
	const char *fld
	)
// STOP FUNC DECL
{
  int status = 0;
  char **Y = NULL; int nY = 0; bool is_multiple = false;

  for ( int i = 0; ; i++ ) { 
    if ( fld[i] == '\0' ) { break; }
    if ( fld[i] == ':' ) { is_multiple = true; break; }
  }
  if ( is_multiple ) {
    status = break_str(fld, ":", &Y, &nY); cBYE(status);
    for ( int i = 0; i < nY; i++ ) { 
      status = del_fld(tbl, -1, Y[i], -1, true); cBYE(status);
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  else {
    status = del_fld(tbl, -1, fld, -1, true); cBYE(status);
  }
BYE:
  return(status);
}
