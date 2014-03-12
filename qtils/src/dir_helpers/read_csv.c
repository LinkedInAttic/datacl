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
#include <sys/mman.h>
#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "open_temp_file.h"
#include "read_meta_data.h"
#include "dbauxil.h"
#include "mk_dict.h"
#include "is_fld.h"
#include "get_nR.h"
#include "read_csv.h" /* contains print_cell() */

/* Given a CSV file and the meta-data for it (as an array), convert the
 * CSV file into the raw files that can be loaded into Q. Since we may
 * want to ignore some of the fields, we provide a string of the form
 * "1:0:1:1" where 1 indicates we want the field an 0 indicates that we
 * wish to ignore it.  If the string is empty or NULL, then we want all
 * fields */

/* Structure of lkp table
 * q describe lkp_bool
 * 1757,text,"",clob,int
 * 2383,idx,"",I4,int
 * 3131,key,"",I8,int
 * 3299,off,"",I4,int
 * 3343,len,"",I4,int
 */

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
char *g_buffer;
int g_buflen;
long long g_num_rows;

// START FUNC DECL
int
read_csv(
	 char *infile,
	 char fld_delim, /* double quote character */
	 char fld_sep, /* comma */
	 char rec_delim, /* new line character */
	 bool ignore_hdr, /* whether to ignore header or not */
	 char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1],
	 int n_flds,
	 FLD_TYPE *fldtype,
	 FLD_PROPS_TYPE *fld_props, /* properties of fields */
	 char fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	 char nn_fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	 HT_REC_TYPE *hts[MAX_NUM_FLDS],
	 int sz_hts[MAX_NUM_FLDS],
	 int n_hts[MAX_NUM_FLDS],
	 int dict_tbl_id[MAX_NUM_FLDS],
	 long long *ptr_num_rows
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0;
  bool in_fld, is_fld_delim;
  long long num_rows = 0; long long num_cells = 0, fld_idx; 
  long long lb, ub;
  FILE *fps[MAX_NUM_FLDS];
  FILE *nn_fps[MAX_NUM_FLDS];
  bool in_escape_mode = false, is_fld_delim_escaped = false;

  char *text_X[MAX_NUM_FLDS]; size_t text_nX[MAX_NUM_FLDS];
  char *key_X [MAX_NUM_FLDS]; size_t key_nX[MAX_NUM_FLDS];
  char *off_X [MAX_NUM_FLDS]; size_t off_nX[MAX_NUM_FLDS];
  char *len_X [MAX_NUM_FLDS]; size_t len_nX[MAX_NUM_FLDS];
  int n_keys[MAX_NUM_FLDS];

  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
    text_X[i] = NULL; text_nX[i] = 0;
    key_X [i] = NULL;  key_nX[i] = 0;
    off_X [i] = NULL;  off_nX[i] = 0;
    len_X [i] = NULL;  len_nX[i] = 0;
  }
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
    long long tmpnR;
    FLD_REC_TYPE tempfldrec, nn_tempfldrec; int itemp, nn_itemp;
    if ( dict_tbl_id[i] >= 0 ) {
      /* text */
      status = is_fld(NULL, dict_tbl_id[i], "txt", &itemp, &tempfldrec,
	  &nn_itemp, &nn_tempfldrec);
      cBYE(status);
      status = get_data(tempfldrec, &(text_X[i]), &(text_nX[i]), false);
      cBYE(status);
      /* key */
      status = is_fld(NULL, dict_tbl_id[i], "key", &itemp, &tempfldrec,
	  &nn_itemp, &nn_tempfldrec);
      cBYE(status);
      status = get_data(tempfldrec, &(key_X[i]), &(key_nX[i]), false);
      cBYE(status);
      /* len */
      status = is_fld(NULL, dict_tbl_id[i], "len", &itemp, &tempfldrec,
	  &nn_itemp, &nn_tempfldrec);
      cBYE(status);
      status = get_data(tempfldrec, &(len_X[i]), &(len_nX[i]), false);
      cBYE(status);
      /* off */
      status = is_fld(NULL, dict_tbl_id[i], "off", &itemp, &tempfldrec,
	  &nn_itemp, &nn_tempfldrec);
      cBYE(status);
      status = get_data(tempfldrec, &(off_X[i]), &(off_nX[i]), false);
      cBYE(status);
      /* number of keys */
      status = int_get_nR(dict_tbl_id[i], &tmpnR); cBYE(status);
      n_keys[i] = tmpnR;
    }
  }
  
  g_num_rows = 0;
  g_buflen = 8192;
  g_buffer = (char *)malloc(g_buflen * sizeof(char));
  return_if_malloc_failed(g_buffer);
  zero_string(g_buffer, g_buflen);

  /*----------------------------------------------------------------------------*/
  if ( n_flds <= 0 ) { go_BYE(-1); }
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
    fps[i] = NULL; 
    nn_fps[i] = NULL; 
  }
  /*----------------------------------------------------------------------------*/
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  lb = 0; fld_idx = 0; in_fld = false; is_fld_delim = false;

  for ( int i = 0; i < n_flds; i++ ) {
    if ( fld_props[i].is_load ) {
      status = open_temp_file(g_cwd, g_data_dir, (fnames[i]), 0); cBYE(status);
      chdir(g_data_dir);
      fps[i] = fopen(fnames[i], "wb");
      return_if_fopen_failed(fps[i], fnames[i],  "wb");
      chdir(g_cwd);
      if ( fld_props[i].is_all_def == false ) { 
        status = open_temp_file(g_cwd, g_data_dir, nn_fnames[i], 0);
        cBYE(status);
        chdir(g_data_dir);
        nn_fps[i] = fopen(nn_fnames[i], "wb");
        return_if_fopen_failed(nn_fps[i], nn_fnames[i],  "wb");
        chdir(g_cwd);
      }
      chdir(g_cwd);
    }
  }
  for ( unsigned long long i = 0; i < nX; i++ ) {
    // START: Error handling for escape conventions
    /* TODO: If I get an incorrect backslash, I simply replace it with a
     * space. This is incorrect. But I need it to read files created 
     * with jspool utility from Jay Thomas. The correct thing to do is
     * to fix the utility.
     */
    if ( fld_sep != '\t' ) {
      if ( in_escape_mode ) { 
	if ( ( X[i] != '\\' ) && ( X[i] != '"' ) ) {
	  fprintf(stderr, "Error. Bad backslash. \n");
	  fprintf(stderr, "num_rows  = %lld \n", num_rows);
	  go_BYE(-1);
	}
      }
    }
    // STOP: Error handling for escape conventions
    // START: Handling bslash and dquote 
    if ( fld_sep != '\t' ) {
      if ( X[i] == '\\' ) { 
	if ( in_escape_mode ) { 
	  in_escape_mode = false;
	}
	else {
	  in_escape_mode = true;
	}
      }
    }
    // STOP: Handling bslash and dquote 
    if ( fld_sep != '\t' ) {
      if ( X[i] == '"' ) { 
	if ( in_escape_mode ) {
	  is_fld_delim_escaped = true;
	  in_escape_mode = false;
	}
      }
    }
    if ( in_fld == false ) {
      in_fld = true;
      if ( X[i] == fld_delim ) { /* consume this character */
	is_fld_delim = true;
        lb = i+1;
      }
      else {
        lb = i; 
	if ( ( ( fld_idx < (n_flds-1) ) && ( X[i] == fld_sep ) ) || 
	     ( ( fld_idx == n_flds-1 ) && ( X[i] == rec_delim ) ) ) {
	  /* field has terminated */
	  ub = i-1;
	  if ( fld_props[fld_idx].is_load ) { 
	    if ( ( ignore_hdr ) && ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(
				  X, lb, ub+1, fldtype[fld_idx], 
				  &(fld_props[fld_idx]), fps[fld_idx], 
				  nn_fps[fld_idx], fld_sep, n_keys[fld_idx], 
				  key_X[fld_idx], text_X[fld_idx], 
				  off_X[fld_idx], len_X[fld_idx], 
				  hts[fld_idx], sz_hts[fld_idx], 
				  &(n_hts[fld_idx]));
	      cBYE(status);
	    }
	  }
	  in_fld = false;
	  num_cells++;
          if ( ( fld_idx == n_flds-1 ) && ( X[i] == rec_delim ) ) {
	    num_rows++;
	    g_num_rows = num_rows;
	  }
	  fld_idx++;
	  if ( fld_idx == n_flds ) { fld_idx = 0; }
	}
      }
    }
    else { /* Now within field. Decide whether to continue or stop. */
      if ( is_fld_delim ) {
	/* if field started with a fld_delim, it must end with one */
	/* Note that we need to take care of escaping the dquote */
	if ( ( X[i] == fld_delim ) && ( !is_fld_delim_escaped ) ) {
	  /* next char must be fld_sep or rec_delim or we have come to eof */
	  if ( i+1 > nX ) { go_BYE(-1); }
	  if ( ( fld_idx < (n_flds-1) ) && ( X[i+1] != fld_sep ) ) { 
	    fprintf(stderr, "num_rows  = %lld \n", num_rows);
	    go_BYE(-1); 
	  }
	  if ( fld_idx == n_flds-1 ) {
	    if ( X[i+1] != rec_delim ) { 
	      go_BYE(-1); 
	    }
	    else {
	      num_rows++;
	      g_num_rows = num_rows;
	    }
	  }
	  ub = i-1;
	  if ( fld_props[fld_idx].is_load ) { 
	    if ( ( ignore_hdr ) && ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(
				  X, lb, ub+1, fldtype[fld_idx], 
				  &(fld_props[fld_idx]), fps[fld_idx], 
				  nn_fps[fld_idx], fld_sep, n_keys[fld_idx], 
				  key_X[fld_idx], text_X[fld_idx], 
				  off_X[fld_idx], len_X[fld_idx], 
				  hts[fld_idx], sz_hts[fld_idx], 
				  &(n_hts[fld_idx]));
	      cBYE(status);
	    }
	  }
	  in_fld = false;
	  is_fld_delim = false;
	  fld_idx++;
	  if ( fld_idx == n_flds ) { fld_idx = 0; }
	  num_cells++;
	  i++; /* Consume next char which must be fld_sep or rec_delim */
	}
      }
      else {
	if ( ( i == nX ) ||  
	     ( ( fld_idx < (n_flds-1) ) && ( X[i] == fld_sep ) ) || 
	     ( ( fld_idx == n_flds-1 ) && ( X[i] == rec_delim ) ) ) {
	  /* field has terminated */
	  if ( i == nX ) { 
	    ub = i;
	  }
	  else {
	    ub = i-1;
	  }
	  if ( fld_props[fld_idx].is_load ) { 
	    if ( ( ignore_hdr ) && ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(
				  X, lb, ub+1, fldtype[fld_idx], 
				  &(fld_props[fld_idx]), fps[fld_idx], 
				  nn_fps[fld_idx], fld_sep, n_keys[fld_idx], 
				  key_X[fld_idx], text_X[fld_idx], 
				  off_X[fld_idx], len_X[fld_idx], 
				  hts[fld_idx], sz_hts[fld_idx], 
				  &(n_hts[fld_idx]));
	      cBYE(status);
	    }
	  }
	  in_fld = false;
	  num_cells++;
          if ( ( fld_idx == n_flds-1 ) && ( X[i] == rec_delim ) ) {
	    num_rows++;
	    g_num_rows = num_rows;
	  }
	  fld_idx++;
	  if ( fld_idx == n_flds ) { fld_idx = 0; }
	}
      }
    }
    is_fld_delim_escaped = false;
  }
  if ( num_cells != ( n_flds * num_rows ) ) { 
    fprintf(stderr, "num_cells = %lld   \n", num_cells);
    fprintf(stderr, "n_flds    = %d   \n", n_flds   );
    fprintf(stderr, "num_rows  = %lld \n", num_rows);
    go_BYE(-1); 
  }
  if ( ignore_hdr ) { 
    num_rows--; // Because you have eliminated first row 
  }
  *ptr_num_rows = num_rows;
 BYE:
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
    fclose_if_non_null(fps[i]);
    fclose_if_non_null(nn_fps[i]);
  }
  if ( status < 0 ) { 
    chdir(g_data_dir);
    for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
      unlink(fnames[i]);
      unlink(nn_fnames[i]);
    }
    chdir(g_cwd);
  }
  rs_munmap(X, nX);
  free_if_non_null(g_buffer);
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) { 
    rs_munmap(text_X[i], text_nX[i]);
    rs_munmap(key_X [i],  key_nX[i]);
    rs_munmap(off_X [i],  off_nX[i]);
    rs_munmap(len_X [i],  len_nX[i]);
  }
  return(status);
}

// START FUNC DECL
int
print_cell(
	   char *X, 
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   FLD_TYPE fldtype,
	   FLD_PROPS_TYPE *ptr_fld_prop,
	   FILE *fp,
	   FILE *nn_fp, 
	   char fld_sep,
	   int n_keys,
	   char *key_X,
	   char *text_X,
	   char *off_X,
	   char *len_X,
	   HT_REC_TYPE *ht,
	   int sz_ht,
	   int *ptr_n_ht
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL;
  int ival4; 
  char ival1; 
  short ival2; 
  long long ival8; 
  unsigned long long uival8; 
  float fval4; 
  double fval8;
  char c_nn;
  int j;
  int n_ht = *ptr_n_ht;

  /*
    fprintf(stderr, "DBG: Extracted : ");
    for ( int i = lb; i < ub; i++ ) {
    fprintf(stderr, "%c", X[i]);
    }
    fprintf(stderr, "\n");
  */
  if ( X == NULL ) { go_BYE(-1); }
  if ( lb > ub ) { go_BYE(-1); }
  if ( fp  == NULL ) { go_BYE(-1); }

  if ( ( ub - lb ) >= g_buflen ) {
    g_buflen *= 2;
    g_buffer = (char *)realloc(g_buffer, g_buflen);
    zero_string(g_buffer, g_buflen);
    fprintf(stderr, "Re-allocating g_buffer\n");
  }
  j = 0;
  for ( long long i = lb; i < ub; i++ ) {
    if ( X[i] == '\\' ) { /* skip over first backslash */
      if ( i == (ub-1) ) { go_BYE(-1); }
      if ( ( X[i] == '\\' ) || ( X[i] == '"' ) ) { 
	/* all is well */
      }
      else {
	go_BYE(-1);
      }
      i++;
    }
    g_buffer[j++] = X[i];
  }
  g_buffer[j] = '\0';  // null terminate

  /*--- START: Determine out nn value */
  if ( lb == ub ) { /* null value */
    if ( ptr_fld_prop->is_all_def ) { 
      fprintf(stderr, "Null value\n");  go_BYE(-1); 
    }
    c_nn = FALSE;
    ptr_fld_prop->is_any_null = true;
  }
  else if ( ub < lb ) {
    go_BYE(-1);
  }
  else {
    c_nn = TRUE;
  }
  /* Note that we defer the write of out_nn since it may change
   * depending on is_null_null and the value of the string */
  /*--- STOP : Determine out nn value */

  ival1 = ival2 = ival4 = ival8 = 0;
  fval4 = fval8 = 0.0;

  if ( ptr_fld_prop->is_string ) { 
    if ( lb < ub ) { 
      if ( key_X != NULL ) { /* reading from existing dictionary */
	status = get_from_dict(g_buffer, (long long *)key_X, n_keys, text_X, 
	    (int *)off_X, (int *)len_X, &ival4);
	cBYE(status);
	if ( ival4 < 0 ) {
	  if ( ptr_fld_prop->is_null_if_missing ) {
            c_nn = FALSE;
	    ival4 = 0; // null values are set to 0
	  }
	  else {
	    fprintf(stderr, "Could not find %s in dictionary \n", g_buffer);
	    go_BYE(-1);
	  }
	}
	else {
          c_nn = TRUE;
	}
      }
      else {
        status = add_to_dict(g_buffer, ht, sz_ht, &n_ht, &uival8); cBYE(status);
        c_nn = TRUE;
      }
    }
    else {
      c_nn = FALSE;
      ptr_fld_prop->is_any_null = true;
      uival8 = 0;
    }
    if ( key_X != NULL ) { /* reading from existing dictionary */
      fwrite(&ival4, sizeof(int), 1, fp);
    }
    else {
      fwrite(&uival8, sizeof(unsigned long long), 1, fp);
    }
  }
  else {
    switch ( fldtype ) {
    case I1 : 
    case I2 : 
    case I4 : 
    case I8 : 
      if ( lb < ub ) { 
	if ( strcmp(g_buffer, "null") == 0 ) { 
	  ival8 = 0;
	  c_nn = FALSE;
          ptr_fld_prop->is_any_null = true;
	  if ( ptr_fld_prop->is_all_def ) { 
	    fprintf(stderr, "Null value\n");  go_BYE(-1); 
	  }
	}
	else {
	  ival8 = strtol(g_buffer, &endptr, 10);
	  if ( *endptr != '\0' ) { 
	    fprintf(stderr, "Error converting to int -> %s \n", g_buffer);
	    fprintf(stderr, "Row number = %lld\n", g_num_rows);
	    go_BYE(-1); 
	  }
	}
      }
      else {
	ival8 = 0;
	c_nn = FALSE;
        ptr_fld_prop->is_any_null = true;
      }
      switch ( fldtype ) { 
      case I1 : 
	if ( ival8 > SCHAR_MAX ) { go_BYE(-1); }
	if ( ival8 < SCHAR_MIN ) { go_BYE(-1); }
	ival1 = (char)ival8;
	fwrite(&ival1, sizeof(char), 1, fp);
	break;
      case I2 : 
	if ( ival8 > SHRT_MAX ) { go_BYE(-1); }
	if ( ival8 < SHRT_MIN ) { go_BYE(-1); }
	ival2 = (short)ival8;
	fwrite(&ival2, sizeof(short), 1, fp);
	break;
      case I4 : 
	if ( ival8 > INT_MAX ) { go_BYE(-1); }
	if ( ival8 < INT_MIN ) { go_BYE(-1); }
	ival4 = (int)ival8;
	fwrite(&ival4, sizeof(int), 1, fp);
	break;
      case I8 : 
	fwrite(&ival8, sizeof(long long), 1, fp);
	break;
      default :
	go_BYE(-1);
	break;
      }
      break;
    case F4 : 
      if ( lb < ub ) { 
	fval4 = strtof(g_buffer, &endptr);
	if ( *endptr != '\0' ) { go_BYE(-1); }
      }
      fwrite(&fval4, sizeof(float), 1, fp);
      break;
    case F8 : 
      if ( lb < ub ) { 
	fval8 = strtod(g_buffer, &endptr);
	if ( *endptr != '\0' ) { go_BYE(-1); }
      }
      fwrite(&fval8, sizeof(double), 1, fp);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  // fprintf(stderr, "[%s]:%d\n", g_buffer, c_nn);
  if ( ptr_fld_prop->is_all_def == false ) { 
    fwrite(&c_nn, sizeof(char), 1, nn_fp);
  }
  zero_string_to_nullc(g_buffer);
  *ptr_n_ht = n_ht;
BYE:
  return(status);
}
