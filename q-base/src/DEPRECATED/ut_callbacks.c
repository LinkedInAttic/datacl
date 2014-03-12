#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "dbauxil.h"
#include "callbacks.h"
#include "is_tbl.h"
#include "add_tbl.h"
//---------------------------------------------------------------
int
ut_callback_num_rslts(
	char *docroot,
	sqlite3 *in_db,
	char *tbl,
	int tbl_id
    )
{
  int status = 0;
  sqlite3 *db = NULL;
  char qstr[4096]; 
  char *zErrMsg = 0; 
  int rc, n;
  char *sval = NULL;
  //------------------------------------------------
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //------------------------------------------------
  zero_string(qstr, 4096);
  sprintf(qstr, "select count(*) from %s ", tbl);
  rc = sqlite3_exec(db, qstr, callback_num_rslts, &n, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  fprintf(stderr, "Rslt for [%s] = %d \n", qstr, n);
  //------------------------------------------------
  zero_string(qstr, 4096);
  sprintf(qstr, "select count(*) from fld where tbl_id = %d ", tbl_id);
  rc = sqlite3_exec(db, qstr, callback_num_rslts, &n, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  fprintf(stderr, "Num results = %d \n", n);
  //------------------------------------------------
  zero_string(qstr, 4096);
  sprintf(qstr, "select name from tbl where id = %d ", tbl_id);
  rc = sqlite3_exec(db, qstr, callback_sval, &sval, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  fprintf(stderr, "sval = %s \n", sval);
  free_if_non_null(sval);
BYE:
  return(status);
}
int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *docroot;
  char *tbl = NULL;
  bool b_is_tbl; 
  int curr_id, max_id, tbl_id;
  sqlite3 *db = NULL;
  int *X = NULL; int nX = 0;
  int n, rc; char *zErrMsg = NULL;
  char qstr[4096];

  zero_string(qstr, 4096);
  tbl = (char *)malloc(64 * sizeof(char));
  zero_string(tbl, 64);
  docroot = getenv("Q_DOCROOT");
  if ( docroot == NULL ) { go_BYE(-1); }

  status = open_db_if_needed(docroot, NULL, &db);
  cBYE(status);

  strcpy(qstr, "select count(*) from tbl where name = 'xxx' ");
  rc = sqlite3_exec(db, qstr, callback_num_rslts, &n, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  fprintf(stderr, "Rslt for [%s] = %d \n", qstr, n);

  status = ut_callback_num_rslts(docroot, db, "tbl", 0);
  cBYE(status);

  fprintf(stderr, "\n-----------------------------------\n");
  db_get_mult_ival(db, "select id from tbl ", &X, &nX);
  cBYE(status);
  fprintf(stderr, "DBG: nX = %d \n", nX);
  for ( int i = 0; i < nX; i++ ) { fprintf(stderr, "X[%d] = %d \n", i, X[i]); }
  free_if_non_null(X);
  fprintf(stderr, "\n-----------------------------------\n");
  /* START: Create a temporary table xxx */
  rc = sqlite3_exec(db, "drop table t1 ", NULL, 0, &zErrMsg); 
  if ( rc != 0 ) { handle_sqlite_error(); }
  rc = sqlite3_exec(db, "create table t1 (f1 int) ", NULL, 0, &zErrMsg); 
  if ( rc != 0 ) { handle_sqlite_error(); }
  rc = sqlite3_exec(db, "insert into t1 (f1) values (10)", NULL, 0, &zErrMsg);
  if ( rc != 0 ) { handle_sqlite_error(); }
  rc = sqlite3_exec(db, "insert into t1 (f1) values (20)", NULL, 0, &zErrMsg);
  if ( rc != 0 ) { handle_sqlite_error(); }
  rc = sqlite3_exec(db, "insert into t1 (f1) values (30)", NULL, 0, &zErrMsg);
  if ( rc != 0 ) { handle_sqlite_error(); }
  rc = sqlite3_exec(db, "insert into t1 (f1) values (40)", NULL, 0, &zErrMsg);
  if ( rc != 0 ) { handle_sqlite_error(); }
  /* STOP: Create a temporary table xxx */
  
  db_get_mult_ival(db, "select f1 from t1 ", &X, &nX);
  cBYE(status);
  fprintf(stderr, "DBG: nX = %d \n", nX);
  for ( int i = 0; i < nX; i++ ) { fprintf(stderr, "X[%d] = %d \n", i, X[i]); }
  free_if_non_null(X);
  fprintf(stderr, "\n-----------------------------------\n");

  strcpy(tbl, "__system");
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &curr_id);
  cBYE(status);
  fprintf(stderr, "tbl = %s, is_tbl = %d, curr_id = %d, max_id = %d \n", 
      tbl, b_is_tbl, curr_id, max_id);
  
  fprintf(stderr, "\n-----------------------------------\n");
  strcpy(tbl, "xxx");
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &curr_id);
  cBYE(status);
  fprintf(stderr, "tbl = %s, is_tbl = %d, curr_id = %d, max_id = %d \n", 
      tbl, b_is_tbl, curr_id, max_id);

  fprintf(stderr, "\n-----------------------------------\n");
  status = add_tbl(docroot, db, tbl, 0, &tbl_id);
  cBYE(status);
  fprintf(stderr, "\n-----------------------------------\n");
  strcpy(tbl, "xxx");
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &curr_id);
  cBYE(status);
  fprintf(stderr, "tbl = %s, is_tbl = %d, curr_id = %d, max_id = %d \n", 
      tbl, b_is_tbl, curr_id, max_id);
  sqlite3_close(db);
BYE:
  free_if_non_null(tbl);
  return(status);
}
