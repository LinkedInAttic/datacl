#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
is_nn_fld(
	char *tbl,
	char *fld,
	int *ptr_nn_fld_id,
	char *str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, fld_id, nn_fld_id;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  nn_fld_id = g_fld[fld_id].nn_fld_id;
  if ( nn_fld_id >= 0 ) {
    sprintf(str_result, "1,%d", nn_fld_id);
  }
  else {
    strcpy(str_result, "0,-1");
  }
  *ptr_nn_fld_id = nn_fld_id;
  //------------------------------------------------
 BYE:
  return(status);
}
