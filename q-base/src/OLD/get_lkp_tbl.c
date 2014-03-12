#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

/* Consider field f in table t. What we want to know is whether f is
 * foreign key to table t' which is a lookup table */

// START FUNC DECL
int
get_lkp_tbl(
	char *tbl,
	char *fld,
	char *str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, fld_id, lkp_tbl_id;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  lkp_tbl_id = g_fld[fld_id].lkp_tbl_id;
  //------------------------------------------------
  if ( lkp_tbl_id < 0 ) {
    strcpy(str_result, "\"-1\",\"\",\"\"");
  }
  else {
    chk_range(lkp_tbl_id, 0, g_n_tbl);
    sprintf(str_result, "\"%d\",\"%s\",\"%s\"", lkp_tbl_id, 
	g_tbl[lkp_tbl_id].name, g_tbl[lkp_tbl_id].dispname);
    // TODO: Need to escape the display name 
  }
  //------------------------------------------------
 BYE:
  return(status);
}
