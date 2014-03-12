#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "parse_attrs.h"
#include "chk_num_rows.h"
#include "get_nR.h"

int glbl_num_rows;
int glbl_curr_id;
int glbl_max_id;
//----------------------------------------------------------------
static int 
callback(
	 void *NotUsed, 
	 int argc, 
	 char **argv, 
	 char **azColName
	 )
{
  int i;
  fprintf(stderr, "argc = %d \n", argc);
  for( i = 0; i < argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "XNULL");
  }
  return 0;
}

int
add_aux_fld(
	char *docroot,
	sqlite3 *in_db,
	char *tbl,
	char *primary_fld,
	char *fld,
	char *filename,
	char *auxtype /* "nn" or "sz" or "hash" */
	)
{
  int status = 0;
  sqlite3 *db = NULL;
  char qstr[4096]; char buf[32];
  char *zErrMsg = 0;
  int nR, rc, tbl_id, max_fld_id, itemp, primary_fld_id;
  int n_sizeof;
  bool b_is_tbl, b_is_primary_fld, b_is_fld;
  char fldtype[16];

  //------------------------------------------------
  zero_string(fldtype, 16);
  zero_string(buf, 32);
  zero_string(qstr, 4096);
  //------------------------------------------------
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( primary_fld == NULL ) || ( *primary_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( auxtype == NULL ) || ( *auxtype == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------
  if ( ( strcmp(auxtype, "sz") == 0 ) || ( strcmp(auxtype, "hash") == 0 ) ) { 
    strcpy(fldtype, "int");
    n_sizeof = sizeof(int);
  }
  else if ( strcmp(auxtype, "nn") == 0 ) {
    strcpy(fldtype, "char");
    n_sizeof = sizeof(char);
  }
  else {
    go_BYE(-1); 
  }
  // Check if table exists
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &tbl_id, &itemp);
  cBYE(status);
  if ( b_is_tbl == false ) {
    fprintf(stderr, "No table [%s] in docroot [%s] \n", tbl, docroot);
    go_BYE(-1);
  }
  //--------------------------------------------
  // Check if primary field exists
  status = is_fld(docroot, db, tbl, primary_fld, &b_is_primary_fld, 
      &primary_fld_id, &itemp);
  cBYE(status);
  if ( b_is_tbl == false ) {
    fprintf(stderr, "No primary field [%s] in docroot [%s] \n", 
	primary_fld, docroot);
    go_BYE(-1);
  }
  //------------------------------------------------
  // Check if field exists
  status = is_fld(docroot, db, tbl, fld, &b_is_fld, &itemp, &max_fld_id);
  cBYE(status);
  if ( b_is_fld == true ) {
    fprintf(stderr, "Auxiliary field [%s] exists in docroot [%s] \n", 
	fld, docroot);
    go_BYE(-1);
  }
  //------------------------------------------------
  // Check that number of rows is consistent
  status = internal_get_nR(db, tbl_id, &nR);
  cBYE(status);
  status = chk_num_rows_2(fldtype, filename, nR);
  cBYE(status);
  //--------------------------------------------
  //-- Now we can add a row to the fld table
  strcpy(qstr, "insert into fld ");
  strcat(qstr, "(id, nR, tbl_id, name, filename, fldtype, auxtype, n_sizeof, parent_id)");
  strcat(qstr, " values (");
  sprintf(buf,"%d, ", max_fld_id); strcat(qstr, buf);
  sprintf(buf,"%d, ", nR); strcat(qstr, buf);
  sprintf(buf,"%d, ", tbl_id); strcat(qstr, buf);
  strcat(qstr, "'"); strcat(qstr, fld); strcat(qstr, "', ");
  strcat(qstr, "'"); strcat(qstr, filename); strcat(qstr, "', ");
  strcat(qstr, "'"); strcat(qstr, fldtype); strcat(qstr, "', ");
  strcat(qstr, "'"); strcat(qstr, auxtype); strcat(qstr, "', ");
  sprintf(buf,"%d, ", n_sizeof); strcat(qstr, buf);
  sprintf(buf,"%d); ", primary_fld_id); strcat(qstr, buf);
  // fprintf(stderr, "qstr = %s \n", qstr);

  rc = sqlite3_exec(db, qstr, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
