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
#include "mmap.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "del_fld.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "meta_globals.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
add_aux_fld(
	char *tbl,
	int in_tbl_id,
	char *fld,
	int in_fld_id,
	int ddir_id,
	char *filename,
	char *str_auxtype, /* "nn" or "sz" */
	int *ptr_aux_fld_id,
	FLD_REC_TYPE *ptr_aux_fld_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  char aux_fld[MAX_LEN_FLD_NAME+1];
  char *X = NULL; size_t nX = 0;
  TBL_REC_TYPE tbl_rec; int tbl_id = INT_MIN;
  FLD_REC_TYPE fld_rec; int fld_id = INT_MIN;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id = INT_MIN;
  AUX_TYPE auxtype;

  zero_fld_rec(&fld_rec);
  zero_fld_rec(&nn_fld_rec);
  //------------------------------------------------
  if ( ( ( tbl == NULL ) || ( *tbl == '\0' ) ) && (in_tbl_id < 0 ) ) {
    go_BYE(-1);
  }
  if ( ( ( fld == NULL ) || ( *fld == '\0' ) ) && (in_fld_id < 0 ) ) {
    go_BYE(-1);
  }
  if ( ( str_auxtype == NULL ) || ( *str_auxtype == '\0' ) ) { go_BYE(-1); }
  if ( ( filename == NULL ) || ( *filename == '\0' ) ) { go_BYE(-1); }
  zero_string(aux_fld, MAX_LEN_FLD_NAME+1);
  //--------------------------------------------

  if ( ( in_tbl_id >= 0 ) && ( in_fld_id >= 0 ) ) { 
    tbl_id = in_tbl_id;
    fld_id = in_fld_id;
    fld_rec = g_flds[fld_id];
    nn_fld_id = g_flds[fld_id].nn_fld_id;
    if ( nn_fld_id >= 0 ) { 
      nn_fld_rec = g_flds[nn_fld_id];
    }
  }
  else if ( ( in_tbl_id < 0 ) || ( in_fld_id < 0 ) ) { 
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    chk_range(tbl_id, 0, g_n_tbl);
    status = is_fld(tbl, -1, fld, &fld_id, &fld_rec, 
      &nn_fld_id, &nn_fld_rec); cBYE(status);
    chk_range(fld_id, 0, g_n_fld);
  }
  else { go_BYE(-1); }
  status = unstr_auxtype(str_auxtype, &auxtype);
  if ( auxtype != nn ) { go_BYE(-1); }
  long long nR = g_tbls[tbl_id].nR;

  status = q_mmap(ddir_id, filename, &X, &nX, false); cBYE(status);
  if ( nR != ( nX * sizeof(char) ) ) { go_BYE(-1); }

  status = mk_name_aux_fld(g_flds[fld_id].name, auxtype, aux_fld); cBYE(status);

  if ( nn_fld_id > 0 ) { 
  }
  else {
    status = get_empty_fld(aux_fld, g_flds, g_n_fld, &nn_fld_id); cBYE(status);
  }
  zero_fld_rec(&(g_flds[nn_fld_id]));

  g_flds[fld_id].nn_fld_id = nn_fld_id; 
  /* Point from auxiliary to primary */
  g_flds[nn_fld_id].parent_id  = fld_id;

  g_flds[nn_fld_id].tbl_id     = tbl_id;
  /*  TODO: I had thought that externality of aux field same 
    as that of parent field. Now, I think not. Look into this */
  g_flds[nn_fld_id].is_external = false; 
  g_flds[nn_fld_id].auxtype    = auxtype;
  g_flds[nn_fld_id].fldtype    = I1; 
  strcpy(g_flds[nn_fld_id].name, aux_fld);
  strcpy(g_flds[nn_fld_id].filename, filename);
  g_flds[nn_fld_id].ddir_id = ddir_id;

  *ptr_aux_fld_id = nn_fld_id;
 BYE:
  rs_munmap(X, nX);
  return(status);
}
