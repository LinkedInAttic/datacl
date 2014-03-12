#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "add_fld.h"
#include "qtypes.h"
#include "auxil.h" 
#include "dbauxil.h"
#include "is_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int
get_nR(
	char *tbl,
	long long *ptr_nR
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  *ptr_nR = g_tbl[tbl_id].nR;
 BYE:
  return(status);
}
