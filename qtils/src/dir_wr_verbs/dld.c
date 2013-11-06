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
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "extract_S.h"
#include "par_f_to_s.h"
#include "dld.h"
#include "read_meta_data.h"
#include "read_csv.h"
#include "set_meta.h"
#include "mk_dict.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "is_a_in_b.h"
#include "ext_s_to_f.h"
#include "f1opf2.h"
#include "open_temp_file.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
#define MAX_LEN 32
// START FUNC DECL
int 
dld(
    char *tbl,
    char *meta_data_file,
    char *data_file,
    char *in_aux_info
    )
// STOP FUNC DECL
{
  int status = 0;
  int n_flds, tbl_id = -1;
  long long num_rows;
  char strbuf[64];
  char fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1];
  char nn_fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1];
#define BUFLEN 32
  char rslt_buf[BUFLEN];

  TBL_REC_TYPE tbl_rec;
  FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS];

  FLD_TYPE fldtype[MAX_NUM_FLDS];
  HT_REC_TYPE *hts[MAX_NUM_FLDS];
  int dict_tbl_id[MAX_NUM_FLDS];
  int sz_hts[MAX_NUM_FLDS];
  int n_hts[MAX_NUM_FLDS];
  char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1];
  char dicts[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1];
  char aux_info[MAX_LEN]; bool is_null;
  bool ignore_hdr = false; // default 
  char fld_delim = '"'; // default 
  char fld_sep = ','; // default 
  FLD_REC_TYPE fld_rec; int fld_id;
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id;
  char opfile[MAX_LEN_FILE_NAME+1];
  int ddir_id[MAX_NUM_FLDS];
  int nn_ddir_id[MAX_NUM_FLDS];

  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  zero_string(rslt_buf, BUFLEN);
  for  ( int i = 0; i < MAX_NUM_FLDS; i++ ) {
    ddir_id[i] = -1; // Will be created in data_dir
    nn_ddir_id[i] = -1; // Will be created in data dir
    dict_tbl_id[i] = INT_MIN;
    hts[i] = NULL;
    sz_hts[i] = 0;
    n_hts[i] = 0;
    zero_string(flds[i], MAX_LEN_FLD_NAME+1);
    zero_string(fnames[i], MAX_LEN_FILE_NAME+1);
    zero_string(nn_fnames[i], MAX_LEN_FILE_NAME+1);
    zero_string(dicts[i], MAX_LEN_FLD_NAME+1);

    fld_props[i].is_load = true;
    fld_props[i].is_all_def = false;
    fld_props[i].is_string = false;
    fld_props[i].is_any_null = false;
    fld_props[i].is_dict_old = false;
    fld_props[i].is_null_null = false;
    fld_props[i].is_null_if_missing = false;
  }


  status = read_meta_data(
			  meta_data_file, flds, fldtype, fld_props,
			  dicts, &n_flds);
  cBYE(status);
  //--- Cannot have same dict for 2 fields in same table during DLD
  for ( int i = 0; i < n_flds; i++ ) {
    if ( strlen(dicts[i]) == 0 ) { continue; }
    for ( int j = i+1; j < n_flds; j++ ) {
      if ( strlen(dicts[j]) == 0 ) { continue; }
      if ( ( fld_props[i].is_dict_old == true ) || 
	  ( fld_props[j].is_dict_old == true ) ) {
	/* Nothing to check */
      }
      else {
        if ( strcmp(dicts[i], dicts[j]) == 0 ) { 
	  fprintf(stderr, "Dictionary [%s] same for rows %d and %d \n",
	      dicts[i], i, j);
	  go_BYE(-1); }
      }
    }
  }
  //---------------------------------------
  // Set up for String Stuff
  if ( ( g_data_dir == NULL ) || ( *g_data_dir == '\0' ) ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( fld_props[i].is_load == false ) {  continue; }
    // skip if not string file 
    if ( fld_props[i].is_string == false ) {  continue; }
    if ( strlen(dicts[i]) == 0 ) { 
      fprintf(stderr, "No dictionary provided for field [%s]\n", flds[i]);
      go_BYE(-1); 
    }
    if ( fld_props[i].is_dict_old ) { 
      status = is_tbl(dicts[i], &(dict_tbl_id[i]), &tbl_rec);
      cBYE(status);
      if (dict_tbl_id[i] < 0 ) { 
        fprintf(stderr, "Could not find dictionary [%s]\n", dicts[i]);
	go_BYE(-1); 
      }
    }
    else { 
      status = setup_ht_dict(&(hts[i]), &(sz_hts[i]), &(n_hts[i])); cBYE(status);
    }
  }
  /*----------------------------------------------------*/
  if ( ( in_aux_info != NULL ) && ( *in_aux_info != '\0' ) ) {
    status = chk_aux_info(in_aux_info); cBYE(status);
    zero_string(aux_info, MAX_LEN);
    status = extract_S(in_aux_info, "ignore_hdr=[", "]", aux_info, MAX_LEN, &is_null);
    if ( is_null ) { 
      ignore_hdr = false;
    }
    else {
      if ( strcasecmp(aux_info, "true") == 0 ) { 
	ignore_hdr = true;
      }
      else if ( strcasecmp(aux_info, "false") == 0 ) { 
	ignore_hdr = false;
      }
      else {
	go_BYE(-1);
      }
    }
    zero_string(aux_info, MAX_LEN);
    status = extract_S(in_aux_info, "fld_sep=[", "]", aux_info, MAX_LEN, &is_null);
    if ( is_null ) { 
      fld_sep = ',';
    }
    else {
      if ( strcasecmp(aux_info, "comma") == 0 ) { 
	fld_sep = ',';
      }
      else if ( strcasecmp(aux_info, "tab") == 0 ) { 
	fld_sep = '\t';
	fld_delim = '\0'; // IMPORTANT!!!!!!
      }
      else if ( strcasecmp(aux_info, "ctrl_z") == 0 ) { 
	fld_sep = 26; /* TODO P1 NEEDS TO BE DEBUGGED */
	fld_delim = '\0'; // IMPORTANT!!!!!!
      }
      else {
	go_BYE(-1);
      }
    }
  }

  status = read_csv(data_file, fld_delim, fld_sep, '\n', ignore_hdr, flds, 
		    n_flds, fldtype, fld_props, fnames, nn_fnames, hts, 
		    sz_hts, n_hts, dict_tbl_id, &num_rows);
  cBYE(status);
  sprintf(strbuf, "%lld", num_rows);
  status = del_tbl(tbl, -1);
  cBYE(status);
  status = add_tbl(tbl, strbuf, &tbl_id, &tbl_rec);
  cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) {
    if( fld_props[i].is_load == false ) { continue; }
    if ( strlen(fnames[i]) == 0 ) { go_BYE(-1); }
    if ( fld_props[i].is_all_def == false ) { 
      if ( strlen(nn_fnames[i]) == 0 ) { go_BYE(-1); }
      // Check whether nn is really needed 
      if ( fld_props[i].is_any_null == false ) {
	unlink(nn_fnames[i]);
	zero_string_to_nullc(nn_fnames[i]);
      }
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    if( fld_props[i].is_load == false ) { continue; }
    if ( strlen(fnames[i]) == 0 ) { go_BYE(-1); }
    if ( fld_props[i].is_string == true ) {
      if ( fld_props[i].is_dict_old == true )  {
        fldtype[i] = I4;
      }
      else {
        fldtype[i] = I8;
      }
    }
    status = add_fld(tbl_id, flds[i], ddir_id[i], fnames[i], 
		     fldtype[i], -1, &fld_id, &fld_rec); 
    cBYE(status);
    if ( strlen(nn_fnames[i]) > 0 ) {
      if ( fld_props[i].is_all_def == true ) { go_BYE(-1); }
      status = add_aux_fld(tbl, tbl_id, flds[i], fld_id, 
			   nn_ddir_id[i], nn_fnames[i], "nn", &nn_fld_id, &nn_fld_rec);
      cBYE(status);
    }
    if ( fld_props[i].is_string == true ) {
      if ( fld_props[i].is_dict_old ) { 
	sprintf(strbuf, "%d", dict_tbl_id[i]);
	status = int_set_meta(tbl_id, fld_id, "dict_tbl_id", strbuf);
	cBYE(status);
        if ( fld_props[i].is_any_null == true ) { go_BYE(-1); }
      }
      else {
	int itemp, dict_tbl_id; FLD_REC_TYPE fldrec; TBL_REC_TYPE tblrec;
	char chrfile[MAX_LEN_FILE_NAME+1];
	char keyfile[MAX_LEN_FILE_NAME+1];
	char lenfile[MAX_LEN_FILE_NAME+1];
	char offfile[MAX_LEN_FILE_NAME+1];
	char *dict_tbl = dicts[i];
	char fk_fld[MAX_LEN_FLD_NAME+1];

	if ( ( strlen(dict_tbl) + strlen("fk_") ) >= MAX_LEN_FLD_NAME ) {
	  go_BYE(-1);
	}
	strcpy(fk_fld, "fk_"); strcat(fk_fld, dict_tbl);
	status = open_temp_file(g_cwd, g_data_dir, chrfile, 0); cBYE(status);
	status = open_temp_file(g_cwd, g_data_dir, keyfile, 0); cBYE(status);
	status = open_temp_file(g_cwd, g_data_dir, lenfile, 0); cBYE(status);
	status = open_temp_file(g_cwd, g_data_dir, offfile, 0); cBYE(status);

	sprintf(strbuf, "%d", n_hts[i]);
	status = add_tbl(dict_tbl, strbuf, &dict_tbl_id, &tblrec); cBYE(status);
	status = set_meta(dict_tbl, NULL, "is_dict_tbl", "true"); cBYE(status);
	status = dump_ht(hts[i], sz_hts[i], n_hts[i], g_data_dir,
			 chrfile, keyfile, lenfile, offfile); 
	cBYE(status);
	status = add_fld(dict_tbl_id, "key", -1, keyfile, I8, -1, &itemp, &fldrec);
	cBYE(status);
	status = add_fld(dict_tbl_id, "len", -1, lenfile, I4, -1, &itemp, &fldrec);
	cBYE(status);
	status = add_fld(dict_tbl_id, "off", -1, offfile, I4, -1, &itemp, &fldrec);
	cBYE(status);
	status = add_fld(dict_tbl_id, "txt", -1, chrfile, clob, -1, &itemp, &fldrec);
	cBYE(status);
	sprintf(strbuf, "%d", dict_tbl_id);
	status = ext_s_to_f(dict_tbl, "idx", 
			    "op=[seq]:start=[0]:incr=[1]:fldtype=[I8]"); 
	cBYE(status);
	status = is_a_in_b(tbl, flds[i], dict_tbl, "key", "", "idx", fk_fld);
	cBYE(status);
	status = f1opf2(tbl, fk_fld, "op=[conv]:newtype=[I4]", fk_fld); 
	cBYE(status);
	status = f1opf2(dict_tbl, "idx", "op=[conv]:newtype=[I4]", "idx"); 
	cBYE(status);
	status = set_meta(tbl, fk_fld, "dict_tbl_id", strbuf);
	cBYE(status);
      }
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( fld_props[i].is_load == false ) {  continue; }
    if ( fld_props[i].is_string == false ) {  continue; }
    if ( hts[i] != NULL ) { 
      for ( int j = 0; j < sz_hts[i]; j++ ) {
        free_if_non_null(hts[i][j].strkey);
      }
      free_if_non_null(hts[i]);
    }
  }
 BYE:
  return(status);
}
