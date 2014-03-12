#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "get_nR.h"
#include "mk_file.h"
#include "assign_char.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
range_select(
	   char *docroot,
	   sqlite3 *in_db,
	   char *src_tbl,
	   char *lb_fld,
	   char *ub_fld,
	   char *dst_tbl,
	   char *dst_cfld
	   )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *lb_fld_X = NULL; size_t lb_fld_nX = 0;
  char *ub_fld_X = NULL; size_t ub_fld_nX = 0;
  FLD_META_TYPE lb_fld_meta, ub_fld_meta;
  long long src_nR; int src_tbl_id;
  long long dst_nR; int dst_tbl_id;
  char str_meta_data[1024];
  char *out_X = NULL; size_t out_nX = 0;
  char *opfile = NULL; FILE *ofp = 0;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld == NULL ) || ( *lb_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_fld == NULL ) || ( *ub_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_cfld == NULL ) || ( *dst_cfld == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_fld_meta(&lb_fld_meta);
  zero_fld_meta(&ub_fld_meta);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, src_tbl, &src_nR, &src_tbl_id);
  cBYE(status);
  status = external_get_nR(docroot, db, dst_tbl, &dst_nR, &dst_tbl_id);
  cBYE(status);
  if ( dst_nR >= INT_MAX ) { 
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
  }
  //--------------------------------------------------------
  status = fld_meta(docroot, db, src_tbl, lb_fld, -1, &lb_fld_meta);
  cBYE(status);
  status = rs_mmap(lb_fld_meta.filename, &lb_fld_X, &lb_fld_nX, 0);
  cBYE(status);
  // TODO: Check that lb is never undefined
  status = fld_meta(docroot, db, src_tbl, ub_fld, -1, &ub_fld_meta);
  cBYE(status);
  status = rs_mmap(ub_fld_meta.filename, &ub_fld_X, &ub_fld_nX, 0);
  cBYE(status);
  // TODO: Check that ub is never undefined
  //--------------------------------------------------------
  if ( strcmp(lb_fld_meta.fldtype, ub_fld_meta.fldtype) != 0 ) {
    fprintf(stderr, "Field types of lb and ub must be same \n");
    go_BYE(-1);
  }
  if ( ( strcmp(lb_fld_meta.fldtype, "int") != 0 ) &&
       ( strcmp(lb_fld_meta.fldtype, "long long") != 0 ) ) {
    fprintf(stderr, "Field types of lb/lb must be int/long long \n");
    go_BYE(-1);
  }
  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, sizeof(bool) * dst_nR);
  cBYE(status);
  rs_mmap(opfile, &out_X, &out_nX, 1);
  cBYE(status);
  int *lbptr = NULL; int *ubptr = NULL; int lb, ub;
  lbptr = (int *)lb_fld_X;
  ubptr = (int *)ub_fld_X;
  for ( long long i = 0; i < src_nR; i++ ) { 
    lb = lbptr[i];
    ub = ubptr[i];
    // Basic Checks
    if ( lb > ub ) { go_BYE(-1); }
    if ( lb < 0 ) { go_BYE(-1); }
    if ( ub > dst_nR ) { go_BYE(-1); }
    // Set values
    assign_const_char(out_X + lb, 1, (ub - lb));
  }
  rs_munmap(out_X, out_nX);
  fclose_if_non_null(ofp);
  //-----------------------------------------------------------
  // Add count field to meta data 
  sprintf(str_meta_data, "fldtype=bool:n_sizeof=1:filename=%s", opfile);
  status = add_fld(docroot, db, dst_tbl, dst_cfld, str_meta_data);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(lb_fld_X, lb_fld_nX);
  rs_munmap(ub_fld_X, ub_fld_nX);
  rs_munmap(out_X, out_nX);
  fclose_if_non_null(ofp);
  free_if_non_null(opfile);
  return(status);
}
