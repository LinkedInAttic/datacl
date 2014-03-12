#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "dbauxil.h"
#include "is_tbl.h"

// START FUNC DECL
int
set_map_tbl(
	char *docroot,
	sqlite3 *in_db,
	char *map_tbl
	)
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char qstr[4096]; 
  bool b_is_tbl;
  int rc, map_tbl_id;
  char *zErrMsg;
  //------------------------------------------------
  if ( ( map_tbl == NULL ) || ( *map_tbl == '\0' ) ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_string(qstr, 4096);
  //------------------------------------------------
  status = is_tbl(docroot, db, map_tbl, &b_is_tbl, &map_tbl_id);
  cBYE(status);
  if ( b_is_tbl == false ) { 
    fprintf(stderr, "tbl [%s] does not exist \n", map_tbl);
    go_BYE(-1);
  }
  //------------------------------------------------
  sprintf(qstr, "update tbl set is_map = 1 where id = %d ", map_tbl_id);
  rc = sqlite3_exec(db, qstr, NULL, 0, &zErrMsg);
  if ( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  //------------------------------------------------
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
