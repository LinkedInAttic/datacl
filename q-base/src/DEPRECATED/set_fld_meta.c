#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "set_fld_meta.h"
#include "aux_fld_meta.h"

/* 
   Parses the string "fld_info" into its component values. 
   */
// START FUNC DECL
int
set_fld_meta(
    char *in_str_fld_info,
    FLD_TYPE *ptr_fld_meta
    )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL, *cptr = NULL, *attr = NULL, *val = NULL;
  char *str_fld_info = in_str_fld_info; 

  // TODO: If I do not make a copy of the string, strsep messes it up
  // Why is this the case?

  for ( ; ; ) { 
    if ( ( str_fld_info == NULL ) || ( *str_fld_info == '\0' ) ) { break; }
    // Find a colon separated string
    cptr = strsep(&str_fld_info, ":");
    if ( cptr == NULL ) { break; }
    // Break it into LHS of equals sign an RHS
    attr = strsep(&cptr, "=");
    if ( attr == NULL ) { go_BYE(-1); }
    val = strsep(&cptr, "=");
    if ( val == NULL ) {  go_BYE(-1); }
    if ( strcmp(attr, "filename") == 0 ) { 
      if ( strlen(val) > MAX_LEN_FILE_NAME ) { go_BYE(-1); }
      strcpy(ptr_fld_meta->filename, val);
    }
    else if ( strcmp(attr, "fldtype") == 0 ) { 
      if ( strlen(val) > MAX_LEN_FLD_TYPE ) { go_BYE(-1); }
      strcpy(ptr_fld_meta->fldtype, val);
    }
    else if ( strcmp(attr, "sort") == 0 ) { 
      if ( strcmp(val, "1") == 0 ) { 
	ptr_fld_meta->sorttype = 1;
      }
      else if ( strcmp(val, "0") == 0 ) { 
	ptr_fld_meta->sorttype = 0;
      }
      else if ( strcmp(val, "-1") == 0 ) { 
	ptr_fld_meta->sorttype = -1;
      }
      else { go_BYE(-1); }
    }
    else if ( strcmp(attr, "is_external") == 0 ) { 
      if ( strcasecmp(val, "true") == 0 ) { 
	ptr_fld_meta->is_external = TRUE;
      }
      else if ( strcasecmp(val, "false") == 0 ) { 
	ptr_fld_meta->is_external = FALSE;
      }
      else { go_BYE(-1); }
    }
    else if ( strcmp(attr, "auxtype") == 0 ) { 
      if ( ( strlen(val) == 0 ) || ( strcmp(val, "primary") == 0 ) ) {
	ptr_fld_meta->auxtype = -1;
      }
      else if ( strcmp(val, "nn") == 0 ) {
	ptr_fld_meta->auxtype = 1;
      }
      else if ( strcmp(val, "sz") == 0 ) {
	ptr_fld_meta->auxtype = 2;
      }
      else { go_BYE(-1); }
    }
    else if ( strcmp(attr, "n_sizeof") == 0 ) { 
      ptr_fld_meta->n_sizeof = strtol(val, &endptr, 10);
    }
    else { 
      fprintf(stderr, "Unknown attribute= [%s] \n", attr);
      go_BYE(-1); 
    }
  }
BYE:
  return(status);
}
