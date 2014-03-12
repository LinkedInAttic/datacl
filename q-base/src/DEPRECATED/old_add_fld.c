#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "sqlite3.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "parse_attrs.h"
#include "chk_num_rows.h"
#include "add_tbl.h"
#include "del_fld.h"

int
add_fld(
	char *docroot,
	sqlite3 *in_db,
	char *tbl,
	FLD_INFO_TYPE fld_info
	)
{
  int status = 0;
  char qstr[4096]; 
  char *zErrMsg = NULL;
  sqlite3 *db = NULL;
  int rc;
  char *attrs_qstr = NULL;
  int fld_id, tbl_id, max_id;
  int n_sizeof = -1;
  int max_fld_id, nR = -1,  parent_id = -1;
  char buf[32];
  bool b_is_tbl, b_is_fld;
  char *X = NULL; size_t nX = 0;
  char fldtype[MAX_LEN_FLD_TYPE];
  char auxtype[MAX_LEN_AUX_TYPE];

  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( attrs == NULL ) { go_BYE(-1); }
  if ( ( filename == NULL ) || ( *filename == '\0' ) ) { go_BYE(-1); }
  zero_string(buf, 32);
  zero_string(qstr, 4096);
  zero_string(fldtype, MAX_LEN_FLD_TYPE);
  zero_string(auxtype, MAX_LEN_AUX_TYPE); // TODO: HOW DOES THIS GET SET?
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  // TODO: HOW DOES nR get set
  //------------------------------------------------
  status = parse_attrs(attrs, &n_sizeof, fldtype);
  cBYE(status);

  //--------------------------------------------
  if ( n_sizeof == 0 ) {
    // This means that it is a variable length field 
  }
  else if ( n_sizeof < 0 ) {  // not specified by user
    if ( strcmp(fldtype, "I") == 0 ) {
      n_sizeof = sizeof(int);
    }
    else if ( ( strcmp(fldtype, "C") == 0 )  ||
		( strcmp(fldtype, "B") == 0 ) ) {
      n_sizeof = sizeof(char);
    }
    else { go_BYE(-1); }
  }

  if ( ( n_sizeof > 0 ) && ( nR < 0 ) ) { 
    status = rs_mmap(filename, &X, &nX, 0);
    cBYE(status);
    nR = nX / n_sizeof;
    rs_munmap(X, nX);
  }
  //--------------------------------------------
  // Check if table exists
  status = is_tbl(docroot, db, tbl, &b_is_tbl, &tbl_id, &max_id);
  cBYE(status);
  if ( b_is_tbl == false ) {
    fprintf(stderr, "WARNING! Adding table [%s]\n", tbl);
    sprintf(buf, "%d", nR);
    status = add_tbl(docroot, db, tbl, buf, &tbl_id);
    cBYE(status);
    zero_string(buf, 32);
  }
  //--------------------------------------------
  // Check if field exists
  status = is_fld(docroot, db, tbl, fld, &b_is_fld, &fld_id, &max_fld_id);
  cBYE(status);
  if ( b_is_fld == true ) { 
    fprintf(stderr, "WARNING! Deleting existing field %s in table [%s] \n",
	fld, tbl);
    status = del_fld(docroot, db, tbl, fld);
    cBYE(status);
    status = is_fld(docroot, db, tbl, fld, &b_is_fld, &fld_id, &max_fld_id);
    cBYE(status);
    if ( b_is_fld == true ) { go_BYE(-1); }

  }
  //--------------------------------------------
  // Check that number of rows is consistent
  status = chk_num_rows_1(db, tbl_id, &nR);
  cBYE(status);
  status = chk_num_rows_2(fldtype, filename, nR);
  cBYE(status);
  //--------------------------------------------
    strcpy(qstr, "insert into fld ");
    strcat(qstr, "(id, nR, tbl_id, name, filename, fldtype, ");
    strcat(qstr, " auxtype, n_sizeof, parent_id) ");
    strcat(qstr, " values (");
    sprintf(buf,"%d, ", max_fld_id); strcat(qstr, buf);
    sprintf(buf,"%d, ", nR); strcat(qstr, buf);
    sprintf(buf,"%d, ", tbl_id); strcat(qstr, buf);
    strcat(qstr, "'"); strcat(qstr, fld); strcat(qstr, "', ");
    strcat(qstr, "'"); strcat(qstr, filename); strcat(qstr, "', ");
    if ( fldtype[0] == '\0' ) { 
      strcat(qstr, " NULL, ");
    }
    else {
      strcat(qstr, "'"); strcat(qstr, fldtype); strcat(qstr, "', ");
    }
    if ( auxtype[0] == '\0' ) { 
      strcat(qstr, " NULL, ");
    }
    else {
      strcat(qstr, "'"); strcat(qstr, auxtype); strcat(qstr, "', ");
    }
    sprintf(buf,"%d, ", n_sizeof); strcat(qstr, buf);
    sprintf(buf,"%d); ", parent_id); strcat(qstr, buf);
    // fprintf(stderr, "qstr = %s \n", qstr);

    rc = sqlite3_exec(db, qstr, NULL, 0, &zErrMsg);
    if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  free_if_non_null(attrs_qstr);
  return(status);
}
