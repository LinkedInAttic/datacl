#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
set_lkp_tbl(
	char *lkp_tbl,
	char *lkp_id, /* Identifies field which is ID field */
	char *lkp_val /* Identifies field which is description field */
	)
// STOP FUNC DECL
{
  int status = 0;
  int lkp_id_id, lkp_val_id, lkp_tbl_id;
  //------------------------------------------------
  if ( ( lkp_tbl == NULL ) || ( *lkp_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( lkp_id == NULL ) || ( *lkp_id == '\0' ) ) { go_BYE(-1); }
  if ( ( lkp_val == NULL ) || ( *lkp_val == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(lkp_id, lkp_val) == 0 ) { go_BYE(-1); } 
  //------------------------------------------------
  status = is_tbl(lkp_tbl, &lkp_tbl_id); cBYE(status);
  chk_range(lkp_tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, lkp_tbl_id, lkp_id, &lkp_id_id); cBYE(status);
  chk_range(lkp_id_id, 0, g_n_fld);
  status = is_fld(NULL, lkp_tbl_id, lkp_val, &lkp_val_id); cBYE(status);
  chk_range(lkp_val_id, 0, g_n_fld);
  //------------------------------------------------
  g_tbl[lkp_tbl_id].is_lkp = true;
  g_fld[lkp_id_id].is_lkp_id = true;
  g_fld[lkp_val_id].is_lkp_val = true;
  //------------------------------------------------
 BYE:
  return(status);
}
