#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
mod_disp_name_fld(
	char *tbl,
	char *internal_name,
	char *dispname
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, fld_id;
  //------------------------------------------------
  for ( char *cptr = dispname; *cptr != '\0'; cptr++ ) { 
    if ( ( *cptr == '"' ) ||  ( *cptr == '\'' ) || ( *cptr == '\\' ) ) { 
      fprintf(stderr, "Display name cannot have squote or dquote or bslash\n");
      go_BYE(-1);
    }
  }
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( internal_name == NULL ) || ( *internal_name == '\0' ) ) { go_BYE(-1); }
  if ( ( dispname == NULL ) || ( *dispname == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL,tbl_id, internal_name, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  //------------------------------------------------
  for ( int i = 0; i < g_n_fld; i++ ) { 
    if ( ( g_fld[i].tbl_id == tbl_id ) && ( g_fld[i].parent_id < 0 ) ) {
      if ( strcmp(g_fld[i].dispname, dispname) == 0 ) { 
        fprintf(stderr, "This display name is in use\n");
        go_BYE(-1);
      }
    }
  }
  strcpy(g_fld[fld_id].dispname, dispname);
 BYE:
  return(status);
}
