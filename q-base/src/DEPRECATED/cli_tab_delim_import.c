#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "fsize.h"

#define BUFLEN 65536

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *infile = NULL;
  int lno, num_flds = 0; char *fld_spec = NULL;
  char *str_fld_spec = NULL, *endptr = NULL, *op_prefix = NULL;
  char linebuf[BUFLEN], fldbuf[BUFLEN];
  FILE *fp = NULL;
  bool is_bad;
  int itemp; long long lltemp;
  FILE **ofps = NULL; char **fnames = NULL;
  FILE **nn_ofps = NULL; char **nn_fnames = NULL;
  FILE **sz_ofps = NULL; char **sz_fnames = NULL;
  bool *is_any_null = NULL;
  bool is_decr_line_count = false;

  zero_string(linebuf, BUFLEN);
  zero_string(fldbuf, BUFLEN);
  if ( argc != 4 ) { 
    fprintf(stderr, "Usage is %s <infile> <fld_specifier> <op_prefix> \n", argv[0]);
    go_BYE(-1);
  }
  infile       = argv[1];
  str_fld_spec = argv[2];
  op_prefix    = argv[3];
  if ( *infile       == '\0' ) { go_BYE(-1); }
  if ( *str_fld_spec == '\0' ) { go_BYE(-1); }
  if ( *op_prefix    == '\0' ) { go_BYE(-1); }

  num_flds = strlen(str_fld_spec);
  fld_spec = (char *)malloc(num_flds * sizeof(char));
  return_if_malloc_failed(fld_spec);
  is_any_null = (bool *)malloc(num_flds * sizeof(bool));
  return_if_malloc_failed(is_any_null);
  for ( int i = 0; i < num_flds; i++ ) { is_any_null[i] = false; }
  num_flds = 0;
  for ( char *cptr = str_fld_spec; *cptr != '\0'; cptr++ ) { 
    switch ( *cptr ) {
    case 'I' : 
      break;
    case 'L' : 
      break;
    case 'S' : 
      break;
    default: 
      go_BYE(-1);
      break;
    }
    fld_spec[num_flds++] = *cptr;
  }
  //-------------------------------------------------------
  ofps = (FILE **) malloc(num_flds * sizeof(FILE *));
  return_if_malloc_failed(ofps);
  for ( int i = 0; i < num_flds; i++ ) { ofps[i] = NULL; }
  fnames = (char **) malloc(num_flds * sizeof(char *));
  return_if_malloc_failed(fnames);
  for ( int i = 0; i < num_flds; i++ ) { fnames[i] = NULL; }
  //-------------------------------------------------------
  nn_ofps = (FILE **) malloc(num_flds * sizeof(FILE *));
  return_if_malloc_failed(nn_ofps);
  for ( int i = 0; i < num_flds; i++ ) { nn_ofps[i] = NULL; }
  nn_fnames = (char **) malloc(num_flds * sizeof(char *));
  return_if_malloc_failed(nn_fnames);
  for ( int i = 0; i < num_flds; i++ ) { nn_fnames[i] = NULL; }
  //-------------------------------------------------------
  sz_ofps = (FILE **) malloc(num_flds * sizeof(FILE *));
  return_if_malloc_failed(sz_ofps);
  for ( int i = 0; i < num_flds; i++ ) { sz_ofps[i] = NULL; }
  sz_fnames = (char **) malloc(num_flds * sizeof(char *));
  return_if_malloc_failed(sz_fnames);
  for ( int i = 0; i < num_flds; i++ ) { sz_fnames[i] = NULL; }
  //-------------------------------------------------------
  //-------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { 
    int len; char buf[16];
    len = strlen(op_prefix) + 16;
    fnames[i] = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(fnames[i]);
    zero_string(fnames[i], len);
    strcpy(fnames[i], op_prefix);
    sprintf(buf, "%d", i);
    strcat(fnames[i], buf);
    ofps[i] = fopen(fnames[i], "wb");
    return_if_fopen_failed(ofps[i],  fnames[i], "wb");
  }
  //-------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { 
    int len; char buf[16];
    len = strlen(op_prefix) + 16;
    nn_fnames[i] = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(nn_fnames[i]);
    zero_string(nn_fnames[i], len);
    strcpy(nn_fnames[i], "_nn_");
    strcat(nn_fnames[i], op_prefix);
    sprintf(buf, "%d", i);
    strcat(nn_fnames[i], buf);
    nn_ofps[i] = fopen(nn_fnames[i], "wb");
    return_if_fopen_failed(nn_ofps[i],  nn_fnames[i], "wb");
  }
  //-------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { 
    if ( fld_spec[i] != 'S' ) { continue; }
    int len; char buf[16];
    len = strlen(op_prefix) + 16;
    sz_fnames[i] = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(sz_fnames[i]);
    zero_string(sz_fnames[i], len);
    strcpy(sz_fnames[i], "_sz_");
    strcat(sz_fnames[i], op_prefix);
    sprintf(buf, "%d", i);
    strcat(sz_fnames[i], buf);
    sz_ofps[i] = fopen(sz_fnames[i], "wb");
    return_if_fopen_failed(sz_ofps[i],  sz_fnames[i], "wb");
  }
  //-------------------------------------------------------
  //-------------------------------------------------------
  fp = fopen(infile, "r");
  return_if_fopen_failed(fp, infile, "r");
  for ( lno = 0; !feof(fp); lno++ ) {
    char *cptr = NULL; int bufptr = 0;
    cptr = fgets(linebuf, BUFLEN, fp);
    if ( cptr == NULL ) { break; }
    if ( lno == 0 ) {  // Skip header line
      // TODO: I do not like this happening automatically. Fix it.
      is_decr_line_count = true;
      zero_string_to_nullc(linebuf);
      continue;
    }
    for ( int fld_idx = 0; fld_idx < num_flds; fld_idx++ ) { 
      int fldptr = 0; char nullc = '\0'; char nn_val = 2;
      // START: Load a cell into the field buffer
      for ( ; ; ) { 
	if ( linebuf[bufptr] == '\t' ) {
	  if ( fld_idx == (num_flds - 1) ) { go_BYE(-1); }
	  bufptr++; // consume this delimiter
	  break;
	}
	if ( linebuf[bufptr] == '\n' ) {
	  if ( fld_idx != (num_flds - 1) ) { go_BYE(-1); }
	  break;
	}
        fldbuf[fldptr++] = linebuf[bufptr++];
      }
      // STOP : Load a cell into the field buffer
      // fprintf(stderr, "Line %d, Field %d = %s \n", lno, fld_idx, fldbuf);
      is_bad = false;
      switch ( fld_spec[fld_idx] ) { 
      case 'L' : 
	if ( fldptr == 0 ) { 
	  nn_val = FALSE;
	  lltemp = 0;
	  is_any_null[fld_idx] = true;
	}
	else {
	  nn_val = TRUE;
  	  lltemp = strtoll(fldbuf, &endptr, 10);
	  if ( *endptr != '\0' ) { is_bad = true; }
	}
	fwrite(&lltemp, sizeof(long long), 1, ofps[fld_idx]);
	fwrite(&nn_val, sizeof(char), 1, nn_ofps[fld_idx]);
	break;
      case 'I' : 
	if ( fldptr == 0 ) { 
	  nn_val = FALSE;
	  itemp = 0;
	  is_any_null[fld_idx] = true;
	}
	else {
	  nn_val = TRUE;
	  itemp = strtol(fldbuf, &endptr, 10);
	  if ( *endptr != '\0' ) { is_bad = true; }
	}
	fwrite(&itemp, sizeof(int), 1, ofps[fld_idx]);
	fwrite(&nn_val, sizeof(char), 1, nn_ofps[fld_idx]);
	break;
      case 'S' : 
	if ( strcmp(fldbuf, "null") == 0 ) {
	  fldptr = 0;
          zero_string_to_nullc(fldbuf);
	  is_any_null[fld_idx] = true;
	}
	if ( fldptr > 0 ) { 
	  fwrite(fldbuf, sizeof(char), fldptr, ofps[fld_idx]);
	  nn_val = TRUE;
	}
	else {
	  nn_val = FALSE;
	}
	fwrite(&nullc, sizeof(char), 1, ofps[fld_idx]);
	fwrite(&nn_val, sizeof(char), 1, nn_ofps[fld_idx]);
	fldptr++; // this is to account for null char being written
	fwrite(&fldptr, sizeof(int), 1, sz_ofps[fld_idx]);
	break; 
      default : 
	go_BYE(-1);
	break;
      }
      if ( is_bad ) { 
	fprintf(stderr, "Bad Data: Line %d, Field %d = %s \n", 
		lno, fld_idx, fldbuf);
	go_BYE(-1); 
      }
      zero_string_to_nullc(fldbuf);
    }
    zero_string_to_nullc(linebuf);
  } 
  if ( is_decr_line_count ) { 
    lno--; 
  }
  // fprintf(stderr, "DBG: Read %d lines \n", lno);
  //-------------------------------------------------------------
  if ( ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(ofps[i]); }
  }
  free_if_non_null(ofps);
  //-------------------------------------------------------------
  if ( nn_ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(nn_ofps[i]); }
  }
  free_if_non_null(nn_ofps);
  //-------------------------------------------------------------
  if ( sz_ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(sz_ofps[i]); }
  }
  free_if_non_null(sz_ofps);
  //-------------------------------------------------------------
  for ( int i = 0; i < num_flds; i++ ) { 
    if ( is_any_null[i] == false ) { 
      unlink(nn_fnames[i]);
    }
  }
  fprintf(stdout, "%d", lno); // number of rows

 BYE:
  //-------------------------------------------------------------
  if ( ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(ofps[i]); }
  }
  free_if_non_null(ofps);
  //-------------------------------------------------------------
  if ( nn_ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(nn_ofps[i]); }
  }
  free_if_non_null(nn_ofps);
  //-------------------------------------------------------------
  if ( sz_ofps != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { fclose_if_non_null(sz_ofps[i]); }
  }
  free_if_non_null(sz_ofps);
  //-------------------------------------------------------------
  //-------------------------------------------------------------
  if ( fnames != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { free_if_non_null(fnames[i]); }
  }
  free_if_non_null(fnames);
  //-------------------------------------------------------------
  if ( nn_fnames != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { free_if_non_null(nn_fnames[i]); }
  }
  free_if_non_null(nn_fnames);
  //-------------------------------------------------------------
  if ( sz_fnames != NULL ) { 
    for ( int i = 0; i < num_flds; i++ ) { free_if_non_null(sz_fnames[i]); }
  }
  free_if_non_null(sz_fnames);
  //-------------------------------------------------------------
  //-------------------------------------------------------------

  fclose_if_non_null(fp);
  free_if_non_null(fld_spec);
  return(status);
}
