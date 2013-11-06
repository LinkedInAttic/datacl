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
#include "aux_meta.h"
#include "meta_globals.h"

extern char *g_docroot;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
// START FUNC DECL
int
dump(
    char *tblfile,
    char *fldfile
	 )
// STOP FUNC DECL
{
  int status = 0;
  FILE *fp = NULL;
  TBL_REC_TYPE *x = NULL; FLD_REC_TYPE *y = NULL;
  char *X = NULL; size_t nX = 0;
  char str_fldtype[32];

  if ( ( tblfile == NULL ) || ( *tblfile == '\0' ) ) {  go_BYE(-1); }
  if ( ( fldfile == NULL ) || ( *fldfile == '\0' ) ) {  go_BYE(-1); }
  if ( strcmp(tblfile, fldfile) == 0 ) { go_BYE(-1); }
  fp = fopen(tblfile, "w");
  return_if_fopen_failed(fp, tblfile, "w");
  for ( int i = 0; i < g_n_tbl; i++ ) { 
    x = &(g_tbls[i]);
    if ( ( x->name == NULL ) || ( x->name[0] == '\0' ) ) {
      continue;
    }
    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", x->name);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */
    fprintf(fp, "\""); /* open quote */
    pr_disp_name(fp, x->dispname);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, "\n");  /* record delimiter */
  }
  fclose_if_non_null(fp);
  /*-------------------------------------------------*/
  fp = fopen(fldfile, "w");
  return_if_fopen_failed(fp, fldfile, "w");
  for ( int i = 0; i < g_n_fld; i++ ) { 
    y = &(g_flds[i]);
    if ( ( y->filename == NULL ) || ( y->filename[0] == '\0' ) ) {
      continue;
    }
    fprintf(fp, "%d,%d,%d,%d,", i, y->tbl_id, y->is_external, y->sort_type);
    if ( y->parent_id < 0 ) { 
      fprintf(fp, "\"\",");
    }
    else {
      fprintf(fp, "%d,", y->parent_id);
    }
    if ( y->nn_fld_id < 0 ) { 
      fprintf(fp, "\"\",");
    }
    else {
      fprintf(fp, "%d,", y->nn_fld_id);
    }
    zero_string(str_fldtype, 32);
    status = mk_str_fldtype(y->fldtype, str_fldtype);
    cBYE(status);
    fprintf(fp, "\"%s\",", str_fldtype); /* fldtype */
    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", y->name);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */

    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", y->filename);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */

    if ( ( y->filename != NULL ) && ( y->filename[0] != '\0' ) ) {
      status = get_data(*y, &X, &nX, false); 
      if ( ( status < 0 ) ||  ( nX == 0 ) ) { 
	status = 0;
        fprintf(fp, ","); /* indicating unknown size */
      }
      else {
        fprintf(fp, "%lld,", (long long)nX);
      }
      rs_munmap(X, nX);
    }
    else {
      fprintf(fp, ","); /* indicating unknown size */
    }

    fprintf(fp, "\""); /* open quote */
    pr_disp_name(fp, y->dispname);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",%d", y->dict_tbl_id); 
    fprintf(fp, "\n");  /* record delimiter */
  }
  fclose_if_non_null(fp);
BYE:
  fclose_if_non_null(fp);
  return(status);
}
