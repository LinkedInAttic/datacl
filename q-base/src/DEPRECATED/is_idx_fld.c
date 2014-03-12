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

// START FUNC DECL
int
is_idx_fld(
	char *tbl,
	char *fld,
	int *ptr_idx_fld_id,
	char *str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, idx_fld_id;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(db, tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  //------------------------------------------------
  sprintf(qstr, "select count(*) from fld where tbl_id = %d and is_idx = 1", 
    tbl_id);
  status = db_get_num_rslts(db, qstr, &cnt);
  cBYE(status);
  if ( cnt == 0 ) { 
    idx_fld_id = 0;
  }
  else if ( cnt == 1 ) { 
    sprintf(qstr, "select id from fld where tbl_id = %d and is_idx = 1 ", 
    tbl_id);
    status = db_get_ival(db, qstr, &idx_fld_id);
    cBYE(status);
  }
  else { go_BYE(-1); }
  sprintf(str_result, "%d", idx_fld_id);
  *ptr_idx_fld_id = idx_fld_id;
  //------------------------------------------------
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
