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
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "auxil.h"
#include "del_fld.h"
#include "add_fld.h"
#include "hash_string.h"
#include "meta_globals.h"

extern char *g_docroot;
extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
get_empty_fld(
    const char *fld,
	      FLD_REC_TYPE *flds,
	      int n_fld,
	      int *ptr_fld_id
	      )
// STOP FUNC DECL
{
  int status = 0;
  unsigned int startidx;
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { 
    // Pick a spot to start searching for an empty spot
    struct timeval Tps; struct timezone Tpf;
    gettimeofday(&Tps, &Tpf); 
    startidx = Tps.tv_usec % n_fld;
  }
  else {
    status = hash_string_UI4(fld, &startidx);
    startidx = startidx % n_fld;
  }


  *ptr_fld_id = INT_MIN; /* no empty spot */
  for ( int i = startidx; i < n_fld; i++ ) { 
    if ( flds[i].name[0] == '\0' ) { 
      *ptr_fld_id = i;
      break;
    }
  }
  if ( *ptr_fld_id < 0 ) {
  for ( int i = 0; i < startidx; i++ ) { 
    if ( flds[i].name[0] == '\0' ) { 
      *ptr_fld_id = i;
      break;
    }
  }
  }
  if ( *ptr_fld_id < 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n");
    go_BYE(-1);
  }

 BYE:
  return(status);
}

// START FUNC DECL
int
add_fld(
	int tbl_id,
	const char *fld,
	int ddir_id,
	char *filename,
	FLD_TYPE fldtype,
	int len,
	int *ptr_fld_id,
	FLD_REC_TYPE *ptr_fld_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  FLD_REC_TYPE fld_rec;    int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 

  //------------------------------------------------
  *ptr_fld_id = -1;
  zero_fld_rec(ptr_fld_rec);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  status = chk_fld_name(fld, false);
  cBYE(status);
  //------------------------------------------------
  if ( ddir_id < 0 ) { 
    chdir(g_data_dir); 
  }
  else {
    char *alt_data_dir = g_ddirs[ddir_id].name;
    if ( ( alt_data_dir == NULL ) || ( *alt_data_dir == '\0' ) ) { go_BYE(-1); }
    chdir(alt_data_dir); 
  }
  status = chk_file_size(filename, g_tbls[tbl_id].nR, fldtype); cBYE(status);
  chdir(g_cwd);
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  if ( fld_id >= 0 ) { 
    status = del_fld(NULL, tbl_id, NULL, fld_id, true);
    cBYE(status);
  }
  else {
    status = get_empty_fld(fld, g_flds, g_n_fld, &fld_id); cBYE(status);
  }
  zero_fld_rec(&(g_flds[fld_id]));
  g_flds[fld_id].tbl_id      = tbl_id;
  g_flds[fld_id].is_external = false;
  g_flds[fld_id].sort_type   = unknown;
  g_flds[fld_id].parent_id   = INT_MIN;
  g_flds[fld_id].nn_fld_id   = INT_MIN;
  g_flds[fld_id].fldtype     = fldtype;
  g_flds[fld_id].auxtype     = undef;
  if ( len < 0 ) { 
    g_flds[fld_id].cnt       = -1;
  }
  else {
    if ( fldtype != clob ) { go_BYE(-1); }
    if ( len < 2 ) { go_BYE(-1); }
    g_flds[fld_id].cnt       = len;
  }


  if ( strlen(filename) > MAX_LEN_FILE_NAME ) { 
    fprintf(stderr, "Filename=[%s] too long > %d characters\n",
	filename, MAX_LEN_FILE_NAME);
    go_BYE(-1); 
  }
  strcpy(g_flds[fld_id].filename, filename);

  g_flds[fld_id].ddir_id = ddir_id;

  if ( strlen(fld) > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
  strcpy(g_flds[fld_id].name, fld);


  *ptr_fld_id = fld_id;
  *ptr_fld_rec = g_flds[fld_id];
 BYE:
  return(status);
}

// START FUNC DECL
int
ext_add_fld(
	const char *tbl,
	const char *fld,
	const char *fldspec
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;    int tbl_id; 
  FLD_REC_TYPE fld_rec;    int fld_id; 
  FLD_REC_TYPE nn_fld_rec;    int nn_fld_id; 
  FLD_TYPE fldtype;
#define MAX_LEN 1024
  char str_len[MAX_LEN]; 
  char str_fldtype[MAX_LEN]; 
  char filename[MAX_LEN]; 
  bool is_null; int len = -1;

  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec);
  if ( fld_id >= 0 ) {
    status = extract_S(fldspec, "filename=[", "]", filename, MAX_LEN, &is_null);
    if ( is_null ){ go_BYE(-1); }
    if ( strcmp(fld_rec.filename, filename) == 0 ) {
      fprintf(stderr, "Need to handle this case\n");
      go_BYE(-1);
    }
    status = del_fld(NULL, tbl_id, NULL, fld_id, true);
  }


  status = extract_S(fldspec, "fldtype=[", "]", str_fldtype, MAX_LEN, &is_null);
  if ( is_null ){ go_BYE(-1); }
  status = unstr_fldtype(str_fldtype, &fldtype); cBYE(status);

  status = extract_S(fldspec, "filename=[", "]", filename, MAX_LEN, &is_null);
  if ( is_null ){ go_BYE(-1); }

  status = extract_S(fldspec, "len=[", "]", str_len, MAX_LEN, &is_null);
  if ( !is_null ) { 
    char *endptr;
    len = strtoll(str_len, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( len < 2 ) { go_BYE(-1); }
  }
  int ddir_id = -1;
  status = add_fld(tbl_id, fld, ddir_id, filename, fldtype, len, &fld_id, &fld_rec);
  cBYE(status);
BYE:
  return(status);
}
