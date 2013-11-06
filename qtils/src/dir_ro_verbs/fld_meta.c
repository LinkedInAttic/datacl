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
#include "is_tbl.h"
#include "meta_globals.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_fld.h"
#include "fld_meta.h"

extern char *g_docroot; 
// START FUNC DECL
int
fld_meta(
	 const char *tbl,
	 const char *fld,
	 const char *what_to_pr,
	 int *ptr_fld_id,
	 bool is_print
	 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1;    TBL_REC_TYPE tbl_rec;
  int fld_id = -1;    FLD_REC_TYPE fld_rec;
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  char str_fldtype[32];
  bool all_digits = true;


  //------------------------------------------------
  *ptr_fld_id = -1;
  zero_string(str_fldtype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }

  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) {
    return(status);
  }
  for ( char *cptr = (char *)fld; *cptr != '\0' ; cptr++ ) { 
    if ( !isdigit(*cptr) ) { all_digits = false; }
  }
  if ( all_digits ) {
    char *endptr;
    fld_id = strtoll(fld, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    chk_range(fld_id, 0, g_n_fld);
    if ( g_flds[fld_id].name[0] == '\0' ) {
      return(status);
    }
    fld_rec = g_flds[fld_id];
  }
  else {
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, 
	&nn_fld_rec);
    cBYE(status);
  }
  *ptr_fld_id = fld_id;
  if ( fld_id < 0 ) {
    return(status);
  }
  if ( is_print == false ) {
    return(status);
  }

  status = mk_str_fldtype(fld_rec.fldtype, str_fldtype); cBYE(status);
  if ( strcmp(what_to_pr, "name") == 0 ) { 
    fprintf(stdout, "%s", fld_rec.name);
  }
  else if ( strcmp(what_to_pr, "filename") == 0 ) { 
    fprintf(stdout, "%s", fld_rec.filename);
  }
  else if ( strcmp(what_to_pr, "dispname") == 0 ) { 
    fprintf(stdout, "%s", fld_rec.dispname);
  }
  else if ( strcmp(what_to_pr, "cnt") == 0 ) { 
    if ( ( fld_rec.fldtype == I1 ) || ( fld_rec.fldtype == B ) ) {
      fprintf(stdout, "%lld", fld_rec.cnt);
    }
    else  {
      fprintf(stdout, "-1");
    }
  }
  else if ( strcmp(what_to_pr, "len") == 0 ) { 
    if ( fld_rec.fldtype == clob ) {
      fprintf(stdout, "%lld", fld_rec.cnt);
    }
    else  {
      fprintf(stdout, "-1");
    }
  }
  else if ( strcmp(what_to_pr, "len") == 0 ) { 
    fprintf(stdout, "%lld", fld_rec.cnt);
  }
  else if ( strcmp(what_to_pr, "fldtype") == 0 ) { 
    fprintf(stdout, "%s", str_fldtype);
  }
  else { 
    fprintf(stderr, "id,name,dispname,filename,fldtype,parent_id,nn_fld_id,dict_tbl_id,sort_type,ext_or_int,cnt,alias_of\n");
    fprintf(stdout, "%d,%s", fld_id, fld_rec.name);
    fprintf(stdout, ",\""); /* open quote */
    pr_disp_name(stdout, fld_rec.dispname);
    fprintf(stdout, "\""); /* close quote */

    fprintf(stdout, ",\"%s\"", fld_rec.filename);

    fprintf(stdout, ",%s,", str_fldtype);
  if ( fld_rec.parent_id < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%d,", fld_rec.parent_id);
  }
  if ( fld_rec.nn_fld_id < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%d,", fld_rec.nn_fld_id);
  }
  if ( fld_rec.dict_tbl_id < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%d,", fld_rec.dict_tbl_id);
  }
  switch ( fld_rec.sort_type ) { 
    case unknown    : fprintf(stdout,"unknown_sort,"); break;
    case ascending  : fprintf(stdout,"ascending,"); break;
    case descending : fprintf(stdout,"descending,"); break;
    case unsorted   : fprintf(stdout,"unsorted,"); break;
    default : go_BYE(-1); break;
  }


  if ( fld_rec.is_external ) { 
    fprintf(stdout, "external,");
  }
  else  {
    fprintf(stdout, "internal,");
  }
  if ( fld_rec.cnt < 0 ) { 
    fprintf(stdout, "\"\",");
  }
  else {
    fprintf(stdout, "%lld,", fld_rec.cnt);
  }
  if ( fld_rec.alias_of_fld_id < 0 ) { 
    fprintf(stdout, "\"\"");
  }
  else {
    fprintf(stdout, "%d", fld_rec.alias_of_fld_id);
  }
  fprintf(stdout, "\n");
  }

  //------------------------------------------------
 BYE:
  return(status);
}

// START FUNC DECL
int
int_get_fld_meta(
		 const char *tbl,
		 int in_tbl_id,
		 const char *fld,
		 int in_fld_id,
		 const char *attr,
		 char *value,
		 int sz
		 )
{
// STOP FUNC DECL
    int status = 0;
    int tbl_id = -1;    TBL_REC_TYPE tbl_rec;
    int fld_id = -1;    FLD_REC_TYPE fld_rec;
    int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;

    //------------------------------------------------
    if ( in_tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
      status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    }
    else {
      tbl_id = in_tbl_id;
    }
    if ( tbl_id < 0 ) { go_BYE(-1); }
    //--------------------------------------------------
    if ( in_fld_id < 0 ) { 
      status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, 
		      &nn_fld_id, &nn_fld_rec);
      cBYE(status);
    }
    else {
      fld_id = in_fld_id;
    }
    if ( fld_id < 0 ) { go_BYE(-1); }
    if ( strcmp(attr, "filename") == 0 ) {
      strcpy(value, g_flds[fld_id].filename);
    }
    else if ( strcmp(attr, "ddir_id") == 0 ) {
      sprintf(value, "%d", g_flds[fld_id].ddir_id);
    }
    else {
      go_BYE(-1);
    }
    //------------------------------------------------
  BYE:
    return(status);
  }
// START FUNC DECL
int
get_fld_meta(
    int fld_id,
    FLD_REC_TYPE *ptr_fld_meta
    )
// STOP FUNC DECL
{
  int status = 0;
  zero_fld_rec(ptr_fld_meta);
  chk_range(fld_id, 0, g_n_fld);
  copy_fld_meta(ptr_fld_meta, g_flds[fld_id]); 
BYE:
  return(status);
}

// START FUNC DECL
int
file_to_fld(
    char *filename,
    int *ptr_fld_id
    )
// STOP FUNC DECL
{

  int status = 0;

  if ( ( filename == NULL ) || ( *filename == '\0' ) ) { 
    return(status);
  }
  *ptr_fld_id = -1;
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( strcmp(g_flds[i].filename, filename) == 0 ) { 
      int tbl_id = g_flds[i].tbl_id;
      char *tbl = g_tbls[tbl_id].name;
      fprintf(stdout, "%s:%s\n", tbl, g_flds[i].name); 
      break;
    }
  }
  return(status);
}
