#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "auxil.h"
#include "glue_files.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
glue_files(
    char **infiles,
    int num_infiles,
    char *opfile
    )
// STOP FUNC DECL
{
  int status = 0;
  FILE **tfp = NULL;
  FILE *ofp = NULL;

  if ( infiles == NULL ) { go_BYE(-1); }
  if ( num_infiles <= 0 ) { go_BYE(-1); }
  tfp = (FILE **)malloc(num_infiles * sizeof(FILE *));
  for ( int i = 0; i < num_infiles; i++ ) { 
    if ( ( infiles[i] == NULL ) || ( infiles[i][0] == '\0' ) ) { go_BYE(-1); }
    tfp[i] = fopen(infiles[i], "r");
    return_if_fopen_failed(tfp[i], infiles[i], "r");
  }
  if ( ( opfile == NULL ) || ( *opfile == '\0' ) ) {
    ofp = stdout;
  }
  else {
    ofp = fopen(opfile, "w");
    return_if_fopen_failed(ofp, opfile, "w");
  }
  for ( int row_idx = 0; ; row_idx++ ) { 
    int num_eof = 0; bool is_eof;
    for ( int col_idx = 0; col_idx < num_infiles; col_idx++ ) {
      status = xfer_val_to_opfile(tfp[col_idx], ofp, &is_eof);
      cBYE(status);
      if ( is_eof ) { 
	num_eof++;
      }
      else {
      if ( col_idx != (num_infiles - 1) ) {
	fprintf(ofp, ",");
      }
      else {
	fprintf(ofp, "\n");
      }
      }
    }
    if ( num_eof == num_infiles ) { 
      // All files have terminated 
      break; 
    }
    else if ( num_eof != 0 ) { 
      // Some files have terminated and others have not
      go_BYE(-1); 
    }
  }
BYE:
  if ( tfp != NULL ) {
    for ( int i = 0; i < num_infiles; i++ ) { 
      fclose_if_non_null(tfp[i]); 
    }
  }
  free_if_non_null(tfp);
  return(status);
}

// START FUNC DECL
int
xfer_val_to_opfile(
    FILE *ifp,
    FILE *ofp,
    bool  *ptr_is_eof
    )
// STOP FUNC DECL
{
  int status = 0;

  if ( feof(ifp ) ) { *ptr_is_eof = true; goto BYE; }
  *ptr_is_eof = false;

  for ( int i = 0; ; i++ ) {
    int ic; char c;
    if ( feof(ifp) ) { go_BYE(-1); }
    c = ic = getc(ifp);
    if ( ( i == 0 ) && ( ic < 0 ) ) {
      *ptr_is_eof = true;
      break;
    }
    // Field must start with a dquote
    if ( ( i == 0 ) && ( c != '"' ) ) { 
      int itemp;
      itemp = feof(ifp);
      go_BYE(-1); 
    }
    putc(c, ofp);
    if ( c == '\\' ) { 
      // there must be a next char 
      // next char must be bslash or dquote
      if ( feof(ifp) ) { go_BYE(-1); }
      c = fgetc(ifp);
      if ( ( c != '\\' ) && ( c != '"' ) ) { go_BYE(-1); }
      putc(c, ofp);
    }
    else {
      if ( ( i > 0 ) && ( c == '"' ) ) {
	/* Reached end of cell value  */
	// Consume the eoln
        if ( feof(ifp) ) { go_BYE(-1); }
        c = fgetc(ifp);
	if ( c != '\n' ) { go_BYE(-1); }
	break;
      }
    }
  }
BYE:
  return(status);
}

