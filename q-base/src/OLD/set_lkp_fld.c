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

/* The field from_fld in the table from_tbl is denotes as a fk 
   to the field in lkp_tbl where is_lkp_id = true */

// START FUNC DECL
int
set_lkp_fld(
	char *from_tbl,
	char *from_fld,
	char *lkp_tbl
	)
// STOP FUNC DECL
{
  int status = 0;
  int from_tbl_id, from_fld_id, lkp_tbl_id;
  //------------------------------------------------
  if ( ( from_tbl == NULL ) || ( *from_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( from_fld == NULL ) || ( *from_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( lkp_tbl == NULL ) || ( *lkp_tbl == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(lkp_tbl, &lkp_tbl_id); cBYE(status);
  chk_range(lkp_tbl_id, 0, g_n_tbl);
  //------------------------------------------------
  status = is_tbl(from_tbl, &from_tbl_id); cBYE(status);
  chk_range(from_tbl_id, 0, g_n_tbl);
  //------------------------------------------------
  status = is_fld(NULL, from_tbl_id, from_fld, &from_fld_id); cBYE(status);
  chk_range(from_fld_id, 0, g_n_fld);
  //------------------------------------------------
  if ( g_tbl[lkp_tbl_id].is_lkp == false ) { go_BYE(-1); }
  g_fld[from_fld_id].lkp_tbl_id = lkp_tbl_id;
 BYE:
  return(status);
}
