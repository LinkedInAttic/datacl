#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "parse_scalar_vals.h"
#include "read_meta_data.h"
#include "read_csv.h"
#include "dbauxil.h"

/* Given a CSV file and the meta-data for it (as an array), convert the
 * CSV file into the raw files that can be loaded into Q. Since we may
 * want to ignore some of the fields, we provide a string of the form
 * "1:0:1:1" where 1 indicates we want the field an 0 indicates that we
 * wish to ignore it.  If the string is empty or NULL, then we want all
 * fields */

char *g_buffer;
int g_buflen;
long long g_num_rows;

#define MAXLINE 256
// START FUNC DECL
int
read_csv(
	 FLD_META_TYPE *flds,
	 int n_flds,
	 char *infile,
	 char *str_load_fld,
	 char fld_delim, /* double quote character */
	 char fld_sep, /* comma */
	 char rec_delim, /* new line character */
	 bool ignore_hdr, /* whether to ignore header or not */
	 bool is_null_null, /* whether to convert "null" to null */
	 char ***ptr_nn_fnames,
	 char ***ptr_sz_fnames,
	 long long *ptr_num_rows
	 )
// STOP FUNC DECL
{
  int status = 0;
  int *load_fld = NULL; int n_load_fld = 0;
  char *X = NULL; size_t nX = 0;
  bool in_fld, is_fld_delim;
  long long num_rows = 0; long long num_cells = 0, fld_idx; 
  long long lb, ub;
  FILE **fps = NULL; char **fnames = NULL;
  FILE **nn_fps = NULL; char **nn_fnames = NULL;
  FILE **sz_fps = NULL; char **sz_fnames = NULL;
  bool in_escape_mode = false, is_fld_delim_escaped = false;
  
  g_num_rows = 0;
  g_buflen = 8192;
  g_buffer = (char *)malloc(g_buflen * sizeof(char));
  return_if_malloc_failed(g_buffer);
  zero_string(g_buffer, g_buflen);

  /*----------------------------------------------------------------------------*/
  if ( n_flds <= 0 ) { go_BYE(-1); }
  fps = (FILE **)malloc(n_flds * sizeof(FILE *));
  return_if_malloc_failed(fps);
  fnames = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(fnames);

  nn_fps = (FILE **)malloc(n_flds * sizeof(FILE *));
  return_if_malloc_failed(nn_fps);
  nn_fnames = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(nn_fnames);

  sz_fps = (FILE **)malloc(n_flds * sizeof(FILE *));
  return_if_malloc_failed(sz_fps);
  sz_fnames = (char **)malloc(n_flds * sizeof(char *));
  return_if_malloc_failed(sz_fnames);

  for ( int i = 0; i < n_flds; i++ ) { 
    fps[i] = NULL;
    nn_fps[i] = NULL;
    sz_fps[i] = NULL;
    fnames[i] = nn_fnames[i] = sz_fnames[i] = NULL;
  }
  /*----------------------------------------------------------------------------*/

  if ( ( str_load_fld == NULL ) || ( *str_load_fld == '\0' ) ) {
    load_fld = (int *)malloc(n_flds * sizeof(int));
    return_if_malloc_failed(load_fld);
    for ( int i = 0; i < n_flds; i++ ) { 
      load_fld[i]= 1;
    }
  }
  status = parse_scalar_vals(str_load_fld, &load_fld, &n_load_fld);
  cBYE(status);
  if ( n_load_fld != n_flds ) { go_BYE(-1); }
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  lb = 0; fld_idx = 0; in_fld = false; is_fld_delim = false;
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( load_fld[i] == TRUE ) { 
      status = open_temp_file(&(fps[i]), &(fnames[i]));
      cBYE(status);
      status = open_temp_file(&(nn_fps[i]), &(nn_fnames[i]));
      cBYE(status);
      if ( strcmp(flds[i].fldtype, "char string" ) == 0 ) {
        status = open_temp_file(&(sz_fps[i]), &(sz_fnames[i]));
	cBYE(status);
      }
    }
  }
  for ( long long i = 0; i < nX; i++ ) {
    // START: Error handling for escape conventions
    /* TODO: If I get an incorrect backslash, I simply replace it with a
     * space. This is incorrect. But I need it to read files created 
     * with jspool utility from Jay Thomas. The correct thing to do is
     * to fix the utility.
     */
    if ( in_escape_mode ) { 
      if ( ( X[i] != '\\' ) && ( X[i] != '"' ) ) {
	/* replace the backslash that put is into escape mode wih a space */
	X[i-1] = ' '; 
      }
    }
    // STOP: Error handling for escape conventions
    // START: Handling bslash and dquote 
    if ( X[i] == '\\' ) { 
      if ( in_escape_mode ) { 
	in_escape_mode = false;
      }
      else {
	in_escape_mode = true;
      }
    }
    // STOP: Handling bslash and dquote 
    if ( X[i] == '"' ) { 
      if ( in_escape_mode ) {
	is_fld_delim_escaped = true;
	in_escape_mode = false;
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
	  if ( load_fld[fld_idx] == 1 ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, flds[fld_idx].fldtype,
				  flds[fld_idx].n_sizeof, is_null_null,
				  fps[fld_idx], nn_fps[fld_idx],
				  sz_fps[fld_idx]);
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
	  if ( load_fld[fld_idx] == 1 ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, flds[fld_idx].fldtype,
				  flds[fld_idx].n_sizeof, is_null_null,
				  fps[fld_idx], nn_fps[fld_idx], sz_fps[fld_idx]);
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
	  if ( load_fld[fld_idx] == 1 ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, flds[fld_idx].fldtype,
				  flds[fld_idx].n_sizeof, is_null_null,
				  fps[fld_idx], nn_fps[fld_idx], sz_fps[fld_idx]);
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
  /* TODO: Copy information into meta data structure */
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( fnames[i] != NULL ) { // since not all fields are loaded
      strcpy(flds[i].filename, fnames[i]);
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    if ( fnames != NULL ) { free_if_non_null(fnames[i]); }
    if ( fps != NULL ) { fclose_if_non_null(fps[i]); }
    if ( nn_fps != NULL ) { fclose_if_non_null(nn_fps[i]); }
    if ( sz_fps != NULL ) { fclose_if_non_null(sz_fps[i]); }
  }
  *ptr_nn_fnames = nn_fnames;
  *ptr_sz_fnames = sz_fnames;
  *ptr_num_rows = num_rows;
 BYE:
  rs_munmap(X, nX);
  free_if_non_null(load_fld);
  free_if_non_null(nn_fps);
  free_if_non_null(sz_fps);
  free_if_non_null(fnames);
  free_if_non_null(fps);
  free_if_non_null(g_buffer);
  return(status);
}

// START FUNC DECL
int
print_cell(
	   char *X, 
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   char *fldtype,
	   int n_sizeof,
	   bool is_null_null,
	   FILE *fp,
	   FILE *nn_fp, 
	   FILE *sz_fp
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL;
  int itemp; char ctemp; long long lltemp; bool btemp; float ftemp;
  char c_nn, nullc = 0;
  long long i;
  bool in_escape_mode = false;

  /*
    fprintf(stderr, "DBG: Extracted : ");
    for ( int i = lb; i < ub; i++ ) {
    fprintf(stderr, "%c", X[i]);
    }
    fprintf(stderr, "\n");
  */

  if ( X == NULL ) { go_BYE(-1); }
  if ( lb > ub ) { go_BYE(-1); }
  if ( fldtype == NULL ) { go_BYE(-1); }
  if ( fp  == NULL ) { go_BYE(-1); }

  if ( ( ub - lb ) >= g_buflen ) {
    g_buflen *= 2;
    g_buffer = realloc(g_buffer, g_buflen);
    zero_string(g_buffer, g_buflen);
    fprintf(stderr, "Re-allocating g_buffer\n");
  }
  for ( i = lb; i < ub; i++ ) {
    g_buffer[i-lb] = X[i];
  }
  g_buffer[i-lb] = '\0'; 

  /*--- START: Determine out nn value */
  if ( lb == ub ) { /* null value */
    c_nn = FALSE;
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

  itemp = 0; lltemp = 0; btemp = false; ctemp = 0;

  if ( strcmp(fldtype, "int") == 0 ) {
    if ( lb < ub ) { 
      itemp = strtol(g_buffer, &endptr, 10);
      if ( *endptr != '\0' ) { 
	fprintf(stderr, "Error converting to int -> %s \n", g_buffer);
	fprintf(stderr, "Row number = %lld\n", g_num_rows);
	go_BYE(-1); 
      }
    }
    fwrite(&itemp, sizeof(int), 1, fp);
  }
  else if ( strcmp(fldtype, "long long") == 0 ) {
    if ( lb < ub ) {
      lltemp = strtoll(g_buffer, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
    }
    fwrite(&lltemp, sizeof(long long), 1, fp);
  }
  else if ( strcmp(fldtype, "bool") == 0 ) {
    if ( lb < ub ) { 
      if ( strlen(g_buffer) != 1 ) { go_BYE(-1); }
      if ( *g_buffer == '0' ) {
	btemp = false;
      }
      else if ( *g_buffer == '1' ) {
	btemp = true;
      }
      else { go_BYE(-1); }
    }
    fwrite(&btemp, sizeof(bool), 1, fp);
  }
  else if ( strcmp(fldtype, "char") == 0 ) {
    if ( lb < ub ) { 
      ctemp = *g_buffer;
    }
    fwrite(&ctemp, sizeof(char), 1, fp);
  }
  else if ( strcmp(fldtype, "float") == 0 ) {
    if ( lb < ub ) { 
      ftemp = strtof(g_buffer, &endptr);
      if ( *endptr != '\0' ) { go_BYE(-1); }
    }
    fwrite(&ftemp, sizeof(float), 1, fp);
  }
  else if ( strcmp(fldtype, "char string") == 0 ) {
    long long sz; 
    sz = ub - lb;
    sz++; // since we plan to write an extra null character
    if ( n_sizeof != 0 ) { /* fixed length field */
      if ( sz > n_sizeof ) { go_BYE(-1); }
    }
    if ( ( is_null_null ) && ( strcmp(g_buffer, "null") == 0 ) ) {
      sz = 1;
      c_nn = FALSE;
    }
    else {
      if ( lb < ub ) {
	int kk = 0;
	for ( int k = lb; k < ub; k++, kk++ ) { 
	  if ( g_buffer[kk] != '\\' ) {
	    fwrite(g_buffer+kk, sizeof(char), 1, fp);
	  }
	  else {
	    if ( in_escape_mode ) { 
	      in_escape_mode = false;
	      fwrite(g_buffer+kk, sizeof(char), 1, fp);
	    }
	    else {
	      sz--; // Since we write one less character
	      in_escape_mode = true;
	    }
	  }
	}
      }
    }
    fwrite(&sz, sizeof(int), 1, sz_fp);
    fwrite(&nullc, sizeof(char), 1, fp); // null termination 
    if ( n_sizeof != 0 ) { /* fixed length field */
      for ( int j = sz; j < n_sizeof; j++ ) { 
        fwrite(&nullc, sizeof(char), 1, fp); // null padding
      }
    }
  }
  else if ( strcmp(fldtype, "double") == 0 ) {
    fprintf(stderr, "ERROR: TODO: fldtype = %s \n", fldtype); go_BYE(-1); 
  }
  else if ( strcmp(fldtype, "wchar_t") == 0 ) {
    fprintf(stderr, "ERROR: TODO: fldtype = %s \n", fldtype); go_BYE(-1); 
  }
  else { 
    fprintf(stderr, "ERROR: fldtype = %s \n", fldtype); go_BYE(-1); 
  }
  fwrite(&c_nn, sizeof(char), 1, nn_fp);
 BYE:
  return(status);
}
