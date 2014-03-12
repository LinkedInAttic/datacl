#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "parse_attrs.h"
#include "auxil.h"

/* 
   Parses the string "in_attrs" into its component values. Only the
   following fields can be set 
  char fldtype[MAX_LEN_FLD_TYPE];
  int n_sizeof;
   
   If input
   string is null or empty, then we return without flagging an error.
   This is considered okay. TODO: Under what circumstances is this ok?
   */
// START FUNC DECL
int
parse_attrs(
    char *in_attrs, 
    int *ptr_n_sizeof,
    char *fldtype
    )
// STOP FUNC DECL
{
  int status = 0;
  char *bak_attrs, *attrs, *cptr, *attr, *val, *endptr;
  int n_sizeof, len;

  // Set default values
  n_sizeof = -1; // indicates variable length field 
  if ( ( in_attrs == NULL ) || ( *in_attrs == '\0' ) ) { 
    return(status);
  }
  /* Make a copy of input attributes. Why are we doing this? TODO */
  len = strlen(in_attrs) + 1;
  bak_attrs = attrs = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(attrs);
  zero_string(attrs, len);
  strcpy(attrs, in_attrs);
  
  for ( ; ; ) { 
    if ( ( attrs == NULL ) || ( *attrs == '\0' ) ) { break; }
    // Find a colon separated string
    cptr = strsep(&attrs, ":");
    if ( cptr == NULL ) { break; }
    // Break it into LHS of equals sign an RHS
    attr = strsep(&cptr, "=");
    if ( attr == NULL ) { go_BYE(-1); }
    val = strsep(&cptr, "=");
    if ( val == NULL ) {  go_BYE(-1); }
    if ( strcmp(attr, "sz") == 0 ) { 
      *ptr_n_sizeof = strtol(val, &endptr, 10);
    }
    else if ( strcmp(attr, "fldtype") == 0 ) { 
      strncpy(fldtype, val, MAX_LEN_FLD_TYPE-1);
    }
  }
  // Additional error checking 
BYE:
  free_if_non_null(bak_attrs);
  return(status);
}
