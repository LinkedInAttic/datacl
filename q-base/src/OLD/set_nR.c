#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int
set_nR(
	char *tbl,
	char *str_nR
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  long long nR; char *endptr;
  //------------------------------------------------
  if ( ( str_nR == NULL ) || ( *str_nR == '\0' ) ) { go_BYE(-1); }
  nR = strtoll(str_nR, &endptr, 10);
  if ( nR <= 0 ) { go_BYE(-1);}
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id);
  cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  g_tbl[tbl_id].nR = nR;
 BYE:
  return(status);
}
