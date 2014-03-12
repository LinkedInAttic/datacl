#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "aux_join.h"
#include "get_nR.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

// START FUNC DECL
int
join_prep(
	  char *src_tbl,
	  char *src_lnk, 
	  char *src_val,
	  char *dst_tbl,
	  char *dst_lnk,
	  char *dst_val,
	  FLD_TYPE *ptr_src_val_meta,
	  FLD_TYPE *ptr_src_lnk_meta,
	  FLD_TYPE *ptr_nn_src_val_meta,
	  FLD_TYPE *ptr_nn_src_lnk_meta,
	  FLD_TYPE *ptr_dst_lnk_meta,
	  FLD_TYPE *ptr_nn_dst_lnk_meta,
	  long long *ptr_src_nR,
	  long long *ptr_dst_nR,
	  char **ptr_src_val_X,
	  size_t *ptr_src_val_nX,
	  char **ptr_src_lnk_X,
	  size_t *ptr_src_lnk_nX,
	  char **ptr_nn_src_val_X,
	  size_t *ptr_nn_src_val_nX,
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
  long long nR;
  int src_tbl_id, dst_tbl_id; 
  int src_val_id, src_lnk_id, nn_src_val_id, nn_src_lnk_id;
  int dst_lnk_id, nn_dst_lnk_id;
  FLD_TYPE fld_meta;

  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_val == NULL ) || ( *src_val == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_lnk == NULL ) || ( *dst_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_val == NULL ) || ( *dst_val == '\0' ) ) { go_BYE(-1); }
  if ( ( strcmp(src_tbl, dst_tbl) == 0 ) && 
       ( strcmp(src_val, dst_val) == 0 ) ) {
    go_BYE(-1);
  }
  zero_fld_meta(ptr_src_val_meta);
  if ( ( src_lnk != NULL ) && ( *src_lnk != '\0' ) ) { 
    zero_fld_meta(ptr_src_lnk_meta);
    zero_fld_meta(ptr_nn_src_lnk_meta);
  }
  zero_fld_meta(ptr_nn_src_val_meta);
  zero_fld_meta(ptr_dst_lnk_meta);
  zero_fld_meta(ptr_nn_dst_lnk_meta);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  *ptr_src_nR = nR = g_tbl[src_tbl_id].nR; 
  if ( nR <= 0 ) { go_BYE(-1); }

  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  *ptr_dst_nR = nR = g_tbl[dst_tbl_id].nR; 
  if ( nR <= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------

  status = is_fld(NULL, src_tbl_id, src_val, &src_val_id); cBYE(status);
  chk_range(src_val_id, 0, g_n_fld);
  *ptr_src_val_meta = fld_meta = g_fld[src_val_id];
  status = rs_mmap(fld_meta.filename, ptr_src_val_X, ptr_src_val_nX, 0);
  cBYE(status);
  // Get nn field for source if if it exists
  nn_src_val_id = g_fld[src_val_id].nn_fld_id; 
  if ( nn_src_val_id >= 0 ) { 
    *ptr_nn_src_val_meta = fld_meta = g_fld[nn_src_val_id];
    status = rs_mmap(fld_meta.filename, ptr_nn_src_val_X, ptr_nn_src_val_nX, 0);
    cBYE(status);
  }
  //---------------------------------------------------------------
  if ( ( src_lnk != NULL ) && ( *src_lnk != '\0' ) ) {
    status = is_fld(NULL, src_tbl_id, src_lnk, &src_lnk_id); cBYE(status);
    chk_range(src_lnk_id, 0, g_n_fld);
    *ptr_src_lnk_meta = fld_meta = g_fld[src_lnk_id];
    status = rs_mmap(fld_meta.filename, ptr_src_lnk_X, ptr_src_lnk_nX, 0);
    cBYE(status);
    // Get nn field for source link if if it exists
    nn_src_lnk_id = g_fld[src_lnk_id].nn_fld_id;
    if ( nn_src_lnk_id >= 0 ) { 
      *ptr_nn_src_lnk_meta = fld_meta = g_fld[nn_src_lnk_id];
      status = rs_mmap(fld_meta.filename, ptr_nn_src_lnk_X, 
	  ptr_nn_src_lnk_nX, 0);
      cBYE(status);
    }
  }
  //--------------------------------------------------------
  status = is_fld(NULL, dst_tbl_id, dst_lnk, &dst_lnk_id); cBYE(status);
  chk_range(dst_lnk_id, 0, g_n_fld);
  *ptr_dst_lnk_meta = fld_meta = g_fld[dst_lnk_id];
  status = rs_mmap(fld_meta.filename, ptr_dst_lnk_X, ptr_dst_lnk_nX, 0);
  cBYE(status);
  // Get nn field for source link if if it exists
  nn_dst_lnk_id = g_fld[dst_lnk_id].nn_fld_id;
  if ( nn_dst_lnk_id >= 0 ) { 
    *ptr_nn_dst_lnk_meta = fld_meta = g_fld[nn_dst_lnk_id];
    status = rs_mmap(fld_meta.filename, ptr_nn_dst_lnk_X, ptr_nn_dst_lnk_nX, 0);
    cBYE(status);
  }
 BYE:
  return status ;
}
