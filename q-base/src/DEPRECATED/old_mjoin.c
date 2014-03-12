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
#include "dbauxil.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "bin_search.h"
#include "sort.h"
#include "mjoin.h"
#include "mjoin_int.h"
#include "mjoin_longlong.h"
#include "get_nR.h"

//---------------------------------------------------------------
int 
mjoin(
      char *docroot,
      sqlite3 *in_db,
      char *src_tbl,
      char *src_lnk,
      char *src_fld,
      char *dst_tbl,
      char *dst_lnk,
      char *dst_fld,
      char *op
      )
{
  int status = 0;
  sqlite3 *db = NULL;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  FLD_META_TYPE src_fld_meta, src_lnk_meta;
  FLD_META_TYPE nn_src_fld_meta, nn_src_lnk_meta;
  FLD_META_TYPE dst_lnk_meta;
  FLD_META_TYPE nn_dst_lnk_meta;
  int src_nR, src_tbl_id;
  int dst_nR, dst_tbl_id;
  char str_meta_data[256];
  char *nn_dst_fld = NULL;
  int imjoin_op;
  bool is_any_null; char *nn_opfile = NULL;
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *nn_src_lnk_X = NULL; size_t nn_src_lnk_nX = 0;
  char *nn_dst_lnk_X = NULL; size_t nn_dst_lnk_nX = 0;
  //----------------------------------------------------------------
  zero_string(str_meta_data, 256);
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_lnk == NULL ) || ( *src_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_lnk == NULL ) || ( *dst_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_fld_meta(&src_fld_meta);
  zero_fld_meta(&src_lnk_meta);
  zero_fld_meta(&nn_src_fld_meta);
  zero_fld_meta(&nn_src_lnk_meta);
  zero_fld_meta(&dst_lnk_meta);
  zero_fld_meta(&nn_dst_lnk_meta);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, src_tbl, &src_nR, &src_tbl_id);
  cBYE(status);

  status = fld_meta(docroot, db, src_tbl, src_fld, -1, &src_fld_meta);
  cBYE(status);
  status = rs_mmap(src_fld_meta.filename, &src_fld_X, &src_fld_nX, 0);
  cBYE(status);

  status = fld_meta(docroot, db, src_tbl, src_lnk, -1, &src_lnk_meta);
  cBYE(status);
  status = rs_mmap(src_lnk_meta.filename, &src_lnk_X, &src_lnk_nX, 0);
  cBYE(status);
  // Get nn field for source if if it exists
  status = get_aux_field_if_it_exists(docroot, db, src_tbl, src_fld_meta.id, 
      "nn", &nn_src_fld_meta, &nn_src_fld_X, &nn_src_fld_nX);
  cBYE(status);
  // Get nn field for source link if if it exists
  status = get_aux_field_if_it_exists(docroot, db, src_tbl, src_lnk_meta.id, 
      "nn", &nn_src_lnk_meta, &nn_src_lnk_X, &nn_src_lnk_nX);
  cBYE(status);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, dst_tbl, &dst_nR, &dst_tbl_id);
  cBYE(status);
  status = fld_meta(docroot, db, dst_tbl, dst_lnk, -1, &dst_lnk_meta);
  cBYE(status);
  status = rs_mmap(dst_lnk_meta.filename, &dst_lnk_X, &dst_lnk_nX, 0);
  cBYE(status);

  // Get nn field for dest link if if it exists
  status = get_aux_field_if_it_exists(docroot, db, dst_tbl, dst_lnk_meta.id, 
      "nn", &nn_dst_lnk_meta, &nn_dst_lnk_X, &nn_dst_lnk_nX);
  cBYE(status);

  //--------------------------------------------------------
  status = mk_mjoin_op(op, &imjoin_op);
  cBYE(status);
  //--------------------------------------------------------
  if ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) { 
    // Source field must be integer and link fields must be integer
    if ( strcmp(dst_lnk_meta.fldtype, "int") != 0 ) { go_BYE(-1); }
    if ( strcmp(src_fld_meta.fldtype, "int") != 0 ) { go_BYE(-1); }
    /* Source fldtype = destination fldtype */
    sprintf(str_meta_data, "fldtype=int:n_sizeof=4:filename=");
    status = mjoin_int( src_fld_X, src_lnk_X, dst_lnk_X, nn_src_fld_X,
	  nn_src_lnk_X, nn_dst_lnk_X, src_nR, dst_nR, &is_any_null,
	  str_meta_data, &nn_opfile, imjoin_op);
    cBYE(status);
  }
  else if ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) { 
    // Source field must be long long and link fields must be integer
    if ( strcmp(dst_lnk_meta.fldtype, "long long") != 0 ) { go_BYE(-1); }
    if ( strcmp(src_fld_meta.fldtype, "long long") != 0 ) { go_BYE(-1); }
    /* Source fldtype = destination fldtype */
    sprintf(str_meta_data, "fldtype=long long:n_sizeof=8:filename=");
    status = mjoin_longlong( src_fld_X, src_lnk_X, dst_lnk_X, nn_src_fld_X,
	  nn_src_lnk_X, nn_dst_lnk_X, src_nR, dst_nR, &is_any_null,
	  str_meta_data, &nn_opfile, imjoin_op);
    cBYE(status);
  }
  else {
    go_BYE(-1);
  }

  // Add output field to meta data 
  status = add_fld(docroot, db, dst_tbl, dst_fld, str_meta_data);
  cBYE(status);
  if ( is_any_null == true ) {
    status = add_aux_fld(docroot, db, dst_tbl, dst_fld, nn_opfile, "nn");
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);

  free_if_non_null(nn_dst_fld);
  free_if_non_null(nn_opfile);

  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  rs_munmap(nn_src_lnk_X, nn_src_lnk_nX);
  rs_munmap(nn_dst_lnk_X, nn_dst_lnk_nX);
  return(status);
}
