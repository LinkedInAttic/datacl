#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sqlite3.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "dbauxil.h"
#include "parse_attrs.h"
//---------------------------------------------------------------
int
add_tbl(
	char *docroot,
	sqlite3 *in_db,
	char *tbl,
	char *str_nR,
	int *ptr_tbl_id
	)
{
  int status = 0;
  sqlite3 *db = NULL;
  char qstr[4096]; char buf[32]; 
  char *zErrMsg = 0; char *endptr;
  int rc, nR, curr_id;
  bool b_is_tbl;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_nR == NULL ) || ( *str_nR == '\0' ) ) {
    nR = 0;
  }
  else {
    nR = strtol(str_nR, &endptr, 10);
    if ( nR < 1 ) { go_BYE(-1); }
  }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------
  zero_string(buf, 32);
  zero_string(qstr, 4096);
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &curr_id, ptr_tbl_id);
  cBYE(status);
  //------------------------------------------------
  if ( b_is_tbl == true ) { 
    fprintf(stderr, "WARNING! Deleting tbl [%s]\n", tbl);
    status = del_tbl(docroot, db, tbl);
    cBYE(status);
    status = is_tbl(docroot, db, tbl, &b_is_tbl, &curr_id, ptr_tbl_id);
    cBYE(status);
    if ( b_is_tbl == true ) { go_BYE(-1); }
  }
  strcpy(qstr, "insert into tbl (id, name, nR) values (");
  sprintf(buf, "%d, " , *ptr_tbl_id); strcat(qstr, buf);
  strcat(qstr, "'"); strcat(qstr, tbl); 
  if( nR <= 0 ) {
    strcat(qstr, "', -1); "); 
  }
  else {
    strcat(qstr, "', ");
    sprintf(buf, "%d", nR);
    strcat(qstr, buf);
    strcat(qstr, "); " );
  }
  // fprintf(stderr, "qstr = %s \n", qstr);
  rc = sqlite3_exec(db, qstr, NULL, 0, &zErrMsg);
  if ( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
