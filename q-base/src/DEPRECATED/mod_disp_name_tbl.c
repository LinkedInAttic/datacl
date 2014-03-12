#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int
mod_disp_name_tbl(
	char *tbl,
	char *dispname
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  //------------------------------------------------
  if ( ( dispname == NULL ) || ( *dispname == '\0' ) ) { go_BYE(-1); }
  for ( char *cptr = dispname; *cptr != '\0'; cptr++ ) { 
    if ( ( *cptr == '"' ) ||  ( *cptr == '\'' ) || ( *cptr == '\\' ) ) { 
      fprintf(stderr, "Display name cannot have squote or dquote or bslash\n");
      go_BYE(-1);
    }
  }
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  strcpy(g_tbl[tbl_id].dispname, dispname);
  //------------------------------------------------
 BYE:
  return(status);
}
