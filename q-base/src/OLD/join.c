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
#include "prep_vals.h"
#include "get_nR.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "aux_join.h"
#include "aux_join_elim_null_vals.h"
#include "del_fld.h"
#include "core_join.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
join(
      char *docroot,
      sqlite3 *in_db,
      char *src_tbl,
      char *src_lnk,
      char *src_val,
      char *dst_tbl,
      char *dst_lnk,
      char *dst_fld,
      char *op
      )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *src_val_X = NULL; size_t src_val_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;
  char *src_idx_X = NULL; size_t src_idx_nX = 0;

  char *nn_dst_val_X = NULL; size_t nn_dst_val_nX = 0;
  char *dst_val_X = NULL; size_t dst_val_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  char *dst_idx_X = NULL; size_t dst_idx_nX = 0;
  FLD_META_TYPE src_val_meta, src_lnk_meta, src_idx_meta;
  FLD_META_TYPE dst_val_meta, dst_lnk_meta, dst_idx_meta;
  long long src_nR, dst_nR;  // counts all rows 
  char str_meta_data[1024]; 
  int sizeof_src_idx, sizeof_dst_idx;
  int *iptr;
  int ijoin_op;
  char *x_src_tbl = NULL, *x_dst_tbl = NULL;
  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  status = mk_mjoin_op(op, &ijoin_op);
  cBYE(status);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_fld_meta(&src_val_meta);
  zero_fld_meta(&src_lnk_meta);
  zero_fld_meta(&src_idx_meta);
  zero_fld_meta(&dst_val_meta);
  zero_fld_meta(&dst_lnk_meta);
  zero_fld_meta(&dst_idx_meta);
  //----------------------------------------------------------------
  /* Given the src_tbl, we create x_src_tbl which has no null 
     values in the link fields and which has an index field called "id"
     with values 0, 1, .... |x_src_tbl| -1 */
  status = elim_null_vals(docroot, db, src_tbl, src_lnk, &x_src_tbl);
  cBYE(status);
  /* Similarly for dst_tbl */
  status = elim_null_vals(docroot, db, dst_tbl, dst_lnk, &x_dst_tbl);
  cBYE(status);
  status = sort_if_necessary(docroot, db, x_src_tbl, src_lnk, "id");
  cBYE(status);
  status = sort_if_necessary(docroot, db, x_dst_tbl, dst_lnk, "id");
  cBYE(status);
  // Get meta-data for all necessary fields 
  status = fld_meta(docroot, db, x_src_tbl, src_lnk, -1, &src_lnk_meta);
  cBYE(status);
  status = fld_meta(docroot, db, x_src_tbl, XXXXXXX, -1, &src_idx_meta);
  cBYE(status);
  status = fld_meta(docroot, db, x_src_tbl, src_val, -1, &src_val_meta);
  cBYE(status);

  status = fld_meta(docroot, db, x_dst_tbl, dst_lnk, -1, &dst_lnk_meta);
  cBYE(status);
  status = fld_meta(docroot, db, x_dst_tbl, XXXXXXX, -1, &dst_idx_meta);
  cBYE(status);
  // Get pointer access to all necessary fields
  status = rs_mmap(src_lnk_meta.filename, &src_lnk_X, &src_lnk_nX, 0); 
  cBYE(status);
  status = rs_mmap(src_idx_meta.filename, &src_idx_X, &src_idx_nX, 0); 
  cBYE(status);
  status = rs_mmap(src_val_meta.filename, &src_val_X, &src_val_nX, 0); 
  cBYE(status);

  status = rs_mmap(dst_lnk_meta.filename, &dst_lnk_X, &dst_lnk_nX, 0); 
  cBYE(status);
  status = rs_mmap(dst_idx_meta.filename, &dst_idx_X, &dst_idx_nX, 0); 
  cBYE(status);

  //--------------------------------------------------------
  // Create output data files
  status = open_temp_file(&ofp, &opfile); cBYE(status); 
  fclose_if_non_null(ofp);
  status = open_temp_file(&nn_ofp, &nn_opfile); cBYE(status); 
  fclose_if_non_null(nn_ofp);
  status = mk_file(opfile, src_val_meta.n_sizeof * dst_nR);
  cBYE(status);
  status = mk_file(nn_opfile, sizeof(char)       * dst_nR);
  cBYE(status);
  status = rs_mmap(opfile, &dst_val_X, &dst_val_nX, 1); cBYE(status);
  status = rs_mmap(nn_opfile, &nn_dst_val_X, &nn_dst_val_nX, 1); cBYE(status);
  //--------------------------------------------------------
  // Core join 
  status = core_join_I_I_I_I_I_I(
      (int *)cpy_src_lnk_X, (int *)cpy_src_idx_X, 
      (int *)src_val_X, 
      src_nR, nn_src_nR,
      (int *)cpy_dst_lnk_X, (int *)cpy_dst_idx_X, 
      (int *)dst_val_X, nn_dst_val_X, 
      dst_nR, nn_dst_nR,
      ijoin_op, &is_any_null);
  //--------------------------------------------------------

  // Clean up aux data structures
  rs_munmap(cpy_src_lnk_X, cpy_src_lnk_nX);
  rs_munmap(cpy_src_idx_X, cpy_src_idx_nX);
  rs_munmap(cpy_dst_lnk_X, cpy_dst_lnk_nX);
  rs_munmap(cpy_dst_idx_X, cpy_dst_idx_nX);

  if ( cpy_src_lnk_file == NULL ) { 
    status = del_fld(docroot, db, src_tbl,"_cpy_lnk"); cBYE(status);
  }
  else {
    unlink(cpy_src_lnk_file); free_if_non_null(cpy_src_lnk_file);
  }
  if ( cpy_src_idx_file == NULL ) { 
    status = del_fld(docroot, db, src_tbl,"_cpy_idx"); cBYE(status);
  }
  else {
    unlink(cpy_src_idx_file); free_if_non_null(cpy_src_idx_file);
  }
  if ( cpy_dst_lnk_file == NULL ) { 
    status = del_fld(docroot, db, dst_tbl,"_cpy_lnk"); cBYE(status);
  }
  else {
    unlink(cpy_dst_lnk_file); free_if_non_null(cpy_dst_lnk_file);
  }
  if ( cpy_dst_idx_file == NULL ) { 
    status = del_fld(docroot, db, dst_tbl,"_cpy_idx"); cBYE(status);
  }
  else {
    unlink(cpy_dst_idx_file); free_if_non_null(cpy_dst_idx_file);
  }
  // Add output field to meta data 
  sprintf(str_meta_data,"filename=%s:n_sizeof=%d:fldtype=%s", opfile,
      src_val_meta.n_sizeof, src_val_meta.fldtype);
  status = add_fld(docroot, db, dst_tbl, dst_fld, str_meta_data);
  cBYE(status);
  if ( is_any_null == true ) {
    status = add_aux_fld(docroot, db, dst_tbl, dst_fld, nn_opfile, "nn");
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
    free_if_non_null(nn_opfile);
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(dst_val_X, dst_nX);
  rs_munmap(nn_dst_val_X, nn_dst_nX);

  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);

  rs_munmap(dst_lnk_X, dst_lnk_nX);
  rs_munmap(dst_idx_X, dst_idx_nX);

  free_if_non_null(nn_opfile);
  free_if_non_null(opfile);

  return(status);
}
