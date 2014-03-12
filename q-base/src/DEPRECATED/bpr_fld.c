#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "dbauxil.h"
#include "bpr_fld.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"

#define FLDTYPE_WCHAR_T 100
#define FLDTYPE_CHAR    200
#define DBG_BUF_LEN 32
//---------------------------------------------------------------
// START FUNC DECL
int 
bpr_fld(
       char *docroot,
       sqlite3 *in_db,
       char *tbl,
       char *fld,
       char *cfld,
       char *opfile
       )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL; char qstr[4096];
  FLD_META_TYPE in_fld_meta; 
  char *X = NULL; size_t nX = 0;
  FILE *ofp = NULL;

  //----------------------------------------------------------------
  zero_string(qstr, 4096);
  zero_fld_meta(&in_fld_meta);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( cfld != NULL ) {
    if ( *cfld != '\0' ) { 
      fprintf(stderr, "NOT IMPLEMENTED\n");  go_BYE(-1); 
    }
  }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  status = fld_meta(docroot, db, tbl, fld, -1, &in_fld_meta);
  cBYE(status);
  status = rs_mmap(in_fld_meta.filename, &X, &nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp, opfile, "wb");
  fwrite(X, nX, sizeof(char), ofp);
  fclose_if_non_null(ofp);
  rs_munmap(X, nX);
BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(X, nX);
  fclose_if_non_null(ofp);
  return(status);
}
