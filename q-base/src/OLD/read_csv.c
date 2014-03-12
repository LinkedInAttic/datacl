#include <sys/mman.h>
#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "open_temp_file.h"
#include "parse_scalar_vals.h"
#include "read_meta_data.h"
#include "dbauxil.h"
#include "read_csv.h" /* contains print_cell() */

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
	 char *infile,
	 char fld_delim, /* double quote character */
	 char fld_sep, /* comma */
	 char rec_delim, /* new line character */
	 bool ignore_hdr, /* whether to ignore header or not */
	 char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1],
	 int n_flds,
	 FLD_TYPE *fldtype,
	 bool *is_null_null, /* whether to convert "null" to null */
	 bool *is_load, /* to load or not */
	 bool *is_all_def, /* whether all values are defined */
	 char fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
	 char nn_fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
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
    if ( is_load[i] ) {
      status = open_temp_file((fnames[i]), 0);
      cBYE(status);
      fps[i] = fopen(fnames[i], "wb");
      return_if_fopen_failed(fps[i], fnames[i],  "wb");
      if ( is_all_def[i] == false ) { 
        status = open_temp_file(nn_fnames[i], 0);
        cBYE(status);
        nn_fps[i] = fopen(nn_fnames[i], "wb");
        return_if_fopen_failed(nn_fps[i], nn_fnames[i],  "wb");
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
	  if ( is_load[fld_idx] ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, fldtype[fld_idx], 
		  is_null_null[fld_idx], is_all_def[fld_idx], 
		  fps[fld_idx], nn_fps[fld_idx], fld_sep);
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
	  if ( is_load[fld_idx] ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, fldtype[fld_idx], 
		  is_null_null[fld_idx], is_all_def[fld_idx], 
		  fps[fld_idx], nn_fps[fld_idx], fld_sep);
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
	  if ( is_load[fld_idx] ) { 
	    if ( ( ignore_hdr ) & ( num_rows == 0 ) ) {
	      /* Do nothing */
	    }
	    else {
	      status = print_cell(X, lb, ub+1, fldtype[fld_idx], 
		  is_null_null[fld_idx], is_all_def[fld_idx], 
		  fps[fld_idx], nn_fps[fld_idx], fld_sep);
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
    if ( fps != NULL ) { fclose_if_non_null(fps[i]); }
    if ( nn_fps != NULL ) { fclose_if_non_null(nn_fps[i]); }
  }
  *ptr_num_rows = num_rows;
 BYE:
  rs_munmap(X, nX);
  free_if_non_null(g_buffer);
  return(status);
}

// START FUNC DECL
int
print_cell(
	   char *X, 
	   long long lb, /* inclusive */ 
	   long long ub, /* exclusive */
	   FLD_TYPE fldtype,
	   bool is_null_null,
	   bool is_all_def,
	   FILE *fp,
	   FILE *nn_fp, 
	   char fld_sep
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL;
  int ival4; 
  char ival1; 
  short ival2; 
  long long ival8; 
  float fval4; 
  double fval8;
  char c_nn;
  long long i;

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
    g_buffer = realloc(g_buffer, g_buflen);
    zero_string(g_buffer, g_buflen);
    fprintf(stderr, "Re-allocating g_buffer\n");
  }
  for ( i = lb; i < ub; i++ ) {
    g_buffer[i-lb] = X[i];
  }
  g_buffer[i-lb] = '\0'; 

  if ( strcasecmp(g_buffer, "billing advisor")  == 0 ) { 
    printf("HI\n");
  }

  /*--- START: Determine out nn value */
  if ( lb == ub ) { /* null value */
    if ( is_all_def ) { fprintf(stderr, "Null value\n");  go_BYE(-1); }
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

  ival1 = ival2 = ival4 = ival8 = 0;
  fval4 = fval8 = 0.0;

  switch ( fldtype ) { 
    case I2 : 
    case I4 : 
    case I8 : 
    if ( lb < ub ) { 
      if ( strcmp(g_buffer, "null") == 0 ) { 
	ival4 = 0;
	c_nn = FALSE;
        if ( is_all_def ) { fprintf(stderr, "Null value\n");  go_BYE(-1); }
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
    switch ( fldtype ) { 
      case I2 : 
	if ( ival8 > 65535 ) { go_BYE(-1); }
	ival2 = (short)ival8;
        fwrite(&ival2, sizeof(short), 1, fp);
	break;
      case I4 : 
	if ( ival8 > INT_MAX ) { go_BYE(-1); }
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
    case xstring : 
    fprintf(stderr, "Make call to LevelDB\n");
    go_BYE(-1);
    break;
    default : 
    go_BYE(-1);
    break;
  }
  if ( !is_all_def ) { 
    fwrite(&c_nn, sizeof(char), 1, nn_fp);
  }
 BYE:
  return(status);
}
