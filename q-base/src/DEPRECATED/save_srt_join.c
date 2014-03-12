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
#include "is_tbl.h"
#include "del_fld.h"
#include "core_srt_join_I_I_I.h"
#include "core_srt_join_I_I_L.h"
#include "core_srt_join_I_L_I.h"
#include "core_srt_join_I_L_L.h"
#include "core_srt_join_L_I_I.h"
#include "core_srt_join_L_I_L.h"
#include "core_srt_join_L_L_I.h"
#include "core_srt_join_L_L_L.h"

/* srt_join is used when the source link fields and destination fields are
 * sorted and have no null values */
//---------------------------------------------------------------
// START FUNC DECL
int 
srt_join(
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

  char *nn_dst_val_X = NULL; size_t nn_dst_val_nX = 0;
  char *dst_val_X = NULL; size_t dst_val_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  FLD_META_TYPE src_val_meta, src_lnk_meta;
  FLD_META_TYPE               dst_lnk_meta;
  long long src_nR, dst_nR;  // counts all rows 
  char str_meta_data[1024]; 
  int ijoin_op; bool is_any_null = false;
  int src_tbl_id, dst_tbl_id; bool b_is_tbl;
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
  zero_fld_meta(&dst_lnk_meta);
  //----------------------------------------------------------------
  // Get meta-data for all necessary fields 
  status = is_tbl(docroot, db, src_tbl, &b_is_tbl, &src_tbl_id);
  cBYE(status);
  if ( b_is_tbl == false ) { 
    fprintf(stderr, "tbl [%s] not found\n", src_tbl); go_BYE(-1);
  }
  status = is_tbl(docroot, db, dst_tbl, &b_is_tbl, &dst_tbl_id);
  cBYE(status);
  if ( b_is_tbl == false ) { 
    fprintf(stderr, "tbl [%s] not found\n", src_tbl); go_BYE(-1);
  }

  status = fld_meta(docroot, db, src_tbl, src_lnk, -1, &src_lnk_meta);
  cBYE(status);
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = fld_meta(docroot, db, src_tbl, src_val, -1, &src_val_meta);
    cBYE(status);
  }
  status = fld_meta(docroot, db, dst_tbl, dst_lnk, -1, &dst_lnk_meta);
  cBYE(status);
  status = internal_get_nR(db, src_tbl_id, &src_nR);
  cBYE(status);
  status = internal_get_nR(db, dst_tbl_id, &dst_nR);
  cBYE(status);
  //----------------------------------------------------------------
  // Get pointer access to all necessary fields
  status = rs_mmap(src_lnk_meta.filename, &src_lnk_X, &src_lnk_nX, 0); 
  cBYE(status);
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = rs_mmap(src_val_meta.filename, &src_val_X, &src_val_nX, 0); 
    cBYE(status);
  }
  status = rs_mmap(dst_lnk_meta.filename, &dst_lnk_X, &dst_lnk_nX, 0); 
  cBYE(status);

  //--------------------------------------------------------
  // Create output data files
  status = open_temp_file(&ofp, &opfile); cBYE(status); 
  fclose_if_non_null(ofp);
  status = open_temp_file(&nn_ofp, &nn_opfile); cBYE(status); 
  fclose_if_non_null(nn_ofp);
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = mk_file(opfile, src_val_meta.n_sizeof * dst_nR);
    cBYE(status);
    status = rs_mmap(opfile,    &dst_val_X,    &dst_val_nX,    1); cBYE(status);
  }
  status = mk_file(nn_opfile, sizeof(char)       * dst_nR);
  cBYE(status);
  status = rs_mmap(nn_opfile,    &nn_dst_val_X,    &nn_dst_val_nX,    1); cBYE(status);
  //--------------------------------------------------------
  // Core join 
  if ( ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
       ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
       ( strcmp(dst_lnk_meta.fldtype, "int") == 0 ) ) {
    status = core_srt_join_I_I_I(
				 (int *)src_lnk_X, (int *)src_val_X, src_nR,
				 (int *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "long long") == 0 ) ) {
    status = core_srt_join_I_I_L(
				 (int *)src_lnk_X, (int *)src_val_X, src_nR,
				 (long long *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "int") == 0 ) ) {
    status = core_srt_join_I_L_I(
				 (int *)src_lnk_X, (long long
				   *)src_val_X, src_nR,
				 (int *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "long long") == 0 ) ) {
    status = core_srt_join_I_L_L((int *)src_lnk_X, 
	(long long *)src_val_X, src_nR, (long long *)dst_lnk_X, 
	(long long *)dst_val_X, nn_dst_val_X, 0, dst_nR, ijoin_op, 
	&is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "int") == 0 ) ) {
    status = core_srt_join_L_I_I(
				 (long long *)src_lnk_X, (int
				   *)src_val_X, src_nR,
				 (int *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "int") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "long long") == 0 ) ) {
    status = core_srt_join_L_I_L(
				 (long long *)src_lnk_X, (int
				   *)src_val_X, src_nR,
				 (long long *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "int") == 0 ) ) {
    status = core_srt_join_L_L_I(
				 (long long *)src_lnk_X, (long long
				   *)src_val_X, src_nR,
				 (int *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else if ( ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(src_lnk_meta.fldtype, "long long") == 0 ) && 
	    ( strcmp(dst_lnk_meta.fldtype, "long long") == 0 ) ) {
    status = core_srt_join_L_L_L(
				 (long long *)src_lnk_X, (long long
				   *)src_val_X, src_nR,
				 (long long *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				 0, dst_nR, ijoin_op, &is_any_null);
  }
  else { go_BYE(-1); }
  //--------------------------------------------------------
  // Add output field to meta data 
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
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
  }
  else {
    sprintf(str_meta_data,"filename=%s:n_sizeof=%ld:fldtype=bool", 
	nn_opfile, sizeof(char));
    status = add_fld(docroot, db, dst_tbl, dst_fld, str_meta_data);
    cBYE(status);
  }
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_val_X, dst_val_nX);
  rs_munmap(nn_dst_val_X, nn_dst_val_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);
  free_if_non_null(nn_opfile);
  free_if_non_null(opfile);
  return(status);
}
