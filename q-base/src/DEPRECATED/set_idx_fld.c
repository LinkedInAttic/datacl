#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "is_fld.h"

// START FUNC DECL
int
set_idx_fld(
	char *docroot,
	sqlite3 *in_db,
	char *tbl,
	char *fld
	)
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char qstr[4096]; 
  bool b_exists;
  int rc, tbl_id, fld_id;
  char *zErrMsg;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_string(qstr, 4096);
  //------------------------------------------------
  status = is_tbl_fld(docroot, db, tbl, fld, &tbl_id, &fld_id, &b_exists);
  cBYE(status);
  if ( b_exists == false ) { 
    fprintf(stderr, "No fld [%s] in tbl [%s]\n", fld, tbl);
    go_BYE(-1);
  }
  //------------------------------------------------
  sprintf(qstr, "update fld set is_idx = 1 where id = %d ", fld_id);
  rc = sqlite3_exec(db, qstr, NULL, 0, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  //------------------------------------------------
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
