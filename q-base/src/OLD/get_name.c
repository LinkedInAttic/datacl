#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "meta_globals.h"

// START FUNC DECL
int
get_name(
	int id,
	char *what,
	char *str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  //------------------------------------------------
  if ( ( what == NULL ) || ( *what == '\0' ) ) { go_BYE(-1); }
  if ( ( strcmp(what, "tbl") != 0 ) && ( strcmp(what, "fld") != 0 ) ) {
    go_BYE(-1);
  }
  if ( strcmp(what, "tbl") == 0 ) { 
    chk_range(id, 0, g_n_tbl);
    strcpy(str_result, g_tbl[id].name);
  }
  else if ( strcmp(what, "fld") == 0 ) { 
    chk_range(id, 0, g_n_fld);
    strcpy(str_result, g_fld[id].name);
  }
  else { go_BYE(-1); }
BYE:
  return(status);
}
