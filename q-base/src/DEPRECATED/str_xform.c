#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "str_xform.h"
#include "porter_stemmer.h"
#include "auxil.h"
//---------------------------------------------------------------
// START FUNC DECL
int
str_xform(
    long long nR,
    char *in,
    int *sz_in,
    char *nn_in,
    char *str_op_spec,
    FILE *ofp,
    FILE *nn_ofp,
    FILE *sz_ofp,
    bool *ptr_is_some_null
    )
// STOP FUNC DECL
{
  int status = 0;
  char *cptr = in; char *endptr = NULL;
  char *str_xform_enum = NULL; int xform_enum;
  char *str_maxlen = NULL;     int maxlen;
  int sz, sz_i, buflen = 8192;
  char *buffer = NULL;
  char nullc = '\0'; char nntrue = 1; char nnfalse = 0; int one = 1;

  status = extract_name_value(str_op_spec, "mode=", ":", &str_xform_enum);
  cBYE(status);
  if ( str_xform_enum == NULL ) { go_BYE(-1); }
  status = extract_name_value(str_op_spec, "maxlen=", ":", &str_maxlen);
  cBYE(status);
  if ( str_maxlen == NULL ) { 
    maxlen = INT_MAX; // no truncation
  }
  else {
    maxlen = strtoll(str_maxlen, &endptr, 10);
    if ( maxlen <= 1 ) { go_BYE(-1); }
    if ( *endptr != '\0' ) { go_BYE(-1); }
  }

  *ptr_is_some_null = false;
  if ( strcmp(str_xform_enum, "title_preproc") == 0 ) {
    xform_enum = 1;
  }
  else if ( strcmp(str_xform_enum, "stem") == 0 ) {
    xform_enum = 2;
  }
  else {
    go_BYE(-1);
  }

  buffer = (char *)malloc(buflen * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(buffer, buflen);
  for ( long long i = 0; i < nR; i++ ) { 
    sz_i = sz = sz_in[i];
    if ( sz >= buflen ) {
      for ( ; sz >= buflen; ) { 
	buflen *= 2;
      }
      free_if_non_null(buffer);
      buffer = (char *)malloc(buflen * sizeof(char));
      return_if_malloc_failed(buffer);
      zero_string(buffer, buflen);
    }
    //-----------------------------------------------
    if ( ( nn_in != NULL ) && ( nn_in[i] == 0 ) ) {
      fwrite(&nnfalse, sizeof(char), 1, nn_ofp);
      fwrite(&one, sizeof(int), 1, sz_ofp);
      fwrite(&nullc, sizeof(char), 1, ofp);
    }
    else {
      zero_string_to_nullc(buffer);
      strcpy(buffer, cptr);
      sz--; // to reduce null termination character
      status = clean_buffer(buffer, &sz, xform_enum);
      cBYE(status);
      if ( sz == 0 ) {
	*ptr_is_some_null = true;
      }
      if ( sz > maxlen ) {
	sz = maxlen;
      }
      fwrite(buffer, sizeof(char), sz, ofp);
      sz++; // to put back space for null termination character
      fwrite(&sz, sizeof(int), 1, sz_ofp);
      fwrite(&nullc, sizeof(char), 1, ofp);
      fwrite(&nntrue, sizeof(char), 1, nn_ofp);
    }
    cptr += sz_i;
  }

BYE:
  free_if_non_null(buffer);
  free_if_non_null(str_xform_enum);
  return(status);
}

/* This is the place where we need to do custom cleansing/modifications */
// START FUNC DECL
int
clean_buffer(
	     char *buffer, 
	     int *ptr_bufptr,
	     int xform_enum
	     )
// STOP FUNC DECL
{
  int status = 0;
  int i = 0, j = 0, reduce_by, maxptr;
  int bufptr = *ptr_bufptr;
  char thischar, prevchar;
  if ( buffer == NULL ) { go_BYE(-1); }
  if ( bufptr < 0 ) { go_BYE(-1); }
  if ( bufptr == 0 ) { return(status); }
  prevchar = 'X'; // anything other than space
  switch ( xform_enum ) { 
    case 2 : // Porter's Stemmer
      maxptr = stem(buffer, 0, bufptr-1);
      for ( int j = maxptr+1; j < bufptr; j++ ) { 
        buffer[j] = '\0';
      }
      bufptr = maxptr + 1;
      break;
  case 1 :  // TUNED FOR TITLE EXTRACTION 
    for ( i = 0; i < bufptr; i++ ) {
      thischar = buffer[i];
      if ( isalpha(thischar) ) {
	buffer[j++] = prevchar = tolower(buffer[i]);
      }
      else if ( isspace(thischar) ) {
	if ( prevchar == ' ' ) { /* Ignore this space  */
	}
	else if ( j == 0 ) { 
	  prevchar = ' '; /* Ignore this space as well */
	}
	else {
	  buffer[j++] = prevchar = ' ';
	}
      }
      else if ( ( thischar == '&' ) || ( thischar == '-') || 
              ( thischar == '(' ) || ( thischar == ')') || 
              ( thischar == '[' ) || ( thischar == ']') || 
              ( thischar == '/' ) || ( thischar == ',') || 
              ( thischar == '.' ) || ( thischar == '/') || 
              ( thischar == '\'' ) || ( thischar == '"') ) {
	if ( prevchar == ' ' ) {
	/* Ignore this character */
	}
	else {
	  /* Convert this character into a space */
  	  buffer[j++] = prevchar = ' ';
        }
      }
    }
    reduce_by = bufptr - j;
    for ( ; j < bufptr; j++ ) { 
      buffer[j] = '\0';
    }
    bufptr -= reduce_by;
    break;
  default : 
    go_BYE(-1);
    break;
  }
  *ptr_bufptr = bufptr;
 BYE:
  return(status);
}
