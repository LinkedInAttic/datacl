#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "aux_join.h"
#include "get_nR.h"

// START FUNC DECL
int
join_prep(
	  char *docroot,
	  sqlite3 *in_db,
	  char *src_tbl,
	  char *src_lnk,
	  char *src_fld,
	  char *dst_tbl,
	  char *dst_lnk,
	  char *dst_fld,
	  FLD_META_TYPE *ptr_src_fld_meta,
	  FLD_META_TYPE *ptr_src_lnk_meta,
	  FLD_META_TYPE *ptr_nn_src_fld_meta,
	  FLD_META_TYPE *ptr_nn_src_lnk_meta,
	  FLD_META_TYPE *ptr_dst_lnk_meta,
	  FLD_META_TYPE *ptr_nn_dst_lnk_meta,
	  long long *ptr_src_nR,
	  long long *ptr_dst_nR,
	  char **ptr_src_fld_X,
	  size_t *ptr_src_fld_nX,
	  char **ptr_src_lnk_X,
	  size_t *ptr_src_lnk_nX,
	  char **ptr_nn_src_fld_X,
	  size_t *ptr_nn_src_fld_nX,
	  char **ptr_nn_src_lnk_X,
	  size_t *ptr_nn_src_lnk_nX,
	  char **ptr_dst_lnk_X,
	  size_t *ptr_dst_lnk_nX,
	  char **ptr_nn_dst_lnk_X,
	  size_t *ptr_nn_dst_lnk_nX
	  )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  int src_tbl_id, dst_tbl_id;

  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_lnk == NULL ) || ( *dst_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_fld_meta(ptr_src_fld_meta);
  if ( ( src_lnk != NULL ) && ( *src_lnk != '\0' ) ) { 
    zero_fld_meta(ptr_src_lnk_meta);
    zero_fld_meta(ptr_nn_src_lnk_meta);
  }
  zero_fld_meta(ptr_nn_src_fld_meta);
  zero_fld_meta(ptr_dst_lnk_meta);
  zero_fld_meta(ptr_nn_dst_lnk_meta);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, src_tbl, ptr_src_nR, &src_tbl_id);
  cBYE(status);
  if ( *ptr_src_nR <= 0 ) { go_BYE(-1); }

  status = fld_meta(docroot, db, src_tbl, src_fld, -1, ptr_src_fld_meta);
  cBYE(status);
  status = rs_mmap(ptr_src_fld_meta->filename, ptr_src_fld_X, ptr_src_fld_nX, 0);
  cBYE(status);

  if ( ( src_lnk != NULL ) && ( *src_lnk != '\0' ) ) {
    status = fld_meta(docroot, db, src_tbl, src_lnk, -1, ptr_src_lnk_meta);
    cBYE(status);
    status = rs_mmap(ptr_src_lnk_meta->filename, ptr_src_lnk_X, ptr_src_lnk_nX, 0);
    cBYE(status);
    // Get nn field for source link if if it exists
    status = get_aux_field_if_it_exists(
	docroot, db, src_tbl, ptr_src_lnk_meta->id, "nn", ptr_nn_src_lnk_meta, 
	ptr_nn_src_lnk_X, ptr_nn_src_lnk_nX);
    cBYE(status);
  }
  // Get nn field for source if if it exists
  status = get_aux_field_if_it_exists(
      docroot, db, src_tbl, ptr_src_fld_meta->id, "nn", ptr_nn_src_fld_meta, 
      ptr_nn_src_fld_X, ptr_nn_src_fld_nX);
  cBYE(status);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, dst_tbl, ptr_dst_nR, &dst_tbl_id);
  cBYE(status);
  if ( *ptr_dst_nR <= 0 ) { go_BYE(-1); }
  status = fld_meta(docroot, db, dst_tbl, dst_lnk, -1, ptr_dst_lnk_meta);
  cBYE(status);
  status = rs_mmap(ptr_dst_lnk_meta->filename, ptr_dst_lnk_X, ptr_dst_lnk_nX, 0);
  cBYE(status);

  // Get nn field for dest link if if it exists
  status = get_aux_field_if_it_exists(docroot, db, dst_tbl,
				      ptr_dst_lnk_meta->id, 
				      "nn", ptr_nn_dst_lnk_meta, ptr_nn_dst_lnk_X, ptr_nn_dst_lnk_nX);
  cBYE(status);
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}
