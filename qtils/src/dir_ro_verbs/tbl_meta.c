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
#include "dbauxil.h"
#include "aux_meta.h"
#include "meta_globals.h"
#include "tbl_meta.h"

extern char *g_docroot; 
// START FUNC DECL
int
tbl_meta(
	 char *tbl,
	 bool include_aux,
	 int *fld_ids,
	 int *ptr_num_flds
	 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1; TBL_REC_TYPE tbl_rec;
  char str_fldtype[32];
  int num_flds = 0;

  //------------------------------------------------
  zero_string(str_fldtype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  cBYE(status);
  if ( tbl_id < 0 ) { 
    fprintf(stderr, "Table [%s] not found\n", tbl); 
    go_BYE(-1);
  }
  //------------------------------------------------
  fprintf(stderr,"fld_idx,name,display name,type,is_external,is_lookup,is_alias\n");
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( g_flds[i].tbl_id == tbl_id ) { 
      if ( fld_ids == NULL ) { 
        fprintf(stdout, "%d,%s,\"", i, g_flds[i].name);
        pr_disp_name(stdout, g_flds[i].dispname);
        status = mk_str_fldtype(g_flds[i].fldtype, str_fldtype);
        fprintf(stdout, "\",%s", str_fldtype);
	if ( g_flds[i].is_external ) { 
          fprintf(stdout, ",Y");
	}
	else  {
          fprintf(stdout, ",N");
	}
	if ( g_flds[i].dict_tbl_id < 0 ) { 
          fprintf(stdout, ",N");
	}
	else  {
          fprintf(stdout, ",Y");
	}
	if ( g_flds[i].alias_of_fld_id < 0 ) { 
          fprintf(stdout, ",N");
	}
	else  {
          fprintf(stdout, ",Y");
	}
        fprintf(stdout, "\n");
      }
      if ( fld_ids != NULL ) { 
	if ( g_flds[i].auxtype != undef ) {
	   if ( include_aux ) {
	    fld_ids[num_flds++] = i;
	  }
	}
	else {
          fld_ids[num_flds++] = i;
	}
      }
    }
  }
  if ( ptr_num_flds != NULL ) { *ptr_num_flds = num_flds; }

  //------------------------------------------------
 BYE:
  return(status);
}
// START FUNC DECL
int
get_tbl_meta(
    int tbl_id,
    TBL_REC_TYPE *ptr_tbl_meta
    )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, g_n_tbl);
  copy_tbl_meta(ptr_tbl_meta, g_tbls[tbl_id]); 
BYE:
  return(status);
}

