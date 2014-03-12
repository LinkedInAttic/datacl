#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "xfer_string.h"
#include "xfer_nonstring.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
xfer(
     char *src_tbl,
     char *src_fld,
     char *dst_tbl,
     char *dst_idx, // this is an index into the source table 
     char *dst_fld
     )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *dst_idx_X = NULL; size_t dst_idx_nX = 0;
  FLD_TYPE *src_fld_meta = NULL, *nn_src_fld_meta = NULL,
    *sz_src_fld_meta = NULL;
  FLD_TYPE *dst_idx_meta = NULL;
  FLD_TYPE *nn_dst_idx_meta = NULL;

  long long src_nR, dst_nR;

  char *sz_src_fld_X = NULL; size_t sz_src_fld_nX = 0;
  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *nn_dst_idx_X = NULL; size_t nn_dst_idx_nX = 0;
  char *szX = NULL; size_t n_szX= 0; 
  char *nnX = NULL; size_t n_nnX= 0;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, dst_idx_id = INT_MIN;
  int nn_src_fld_id = INT_MIN, nn_dst_idx_id = INT_MIN;
  int sz_src_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_idx == NULL ) || ( *dst_idx == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) )  { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0); 
  cBYE(status);

  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbl[dst_tbl_id].nR;

  status = is_fld(NULL, dst_tbl_id, dst_idx, &dst_idx_id); cBYE(status);
  chk_range(dst_idx_id, 0, g_n_fld);
  dst_idx_meta = &(g_fld[dst_idx_id]);
  status = rs_mmap(dst_idx_meta->filename, &dst_idx_X, &dst_idx_nX, 0); 
  cBYE(status);

  nn_src_fld_id = src_fld_meta->nn_fld_id;
  if ( nn_src_fld_id >= 0 ) { 
    nn_src_fld_meta = &(g_fld[nn_src_fld_id]);
    status = rs_mmap(nn_src_fld_meta->filename, &nn_src_fld_X,
	  &nn_src_fld_nX, 0); cBYE(status);
  }
  //--------------------------------------------------------
  sz_src_fld_id = src_fld_meta->sz_fld_id;
  if ( strcmp(src_fld_meta->fldtype, "char string") == 0 ) { 
    chk_range(sz_src_fld_id, 0, g_n_fld);
  sz_src_fld_meta = &(g_fld[sz_src_fld_id]);
  status = rs_mmap(sz_src_fld_meta->filename, &sz_src_fld_X,
	  &sz_src_fld_nX, 0); cBYE(status);
  }
  //--------------------------------------------------------
  nn_dst_idx_id = dst_idx_meta->nn_fld_id;
  if ( nn_dst_idx_id >= 0 ) { 
    nn_dst_idx_meta = &(g_fld[nn_dst_idx_id]);
    status = rs_mmap(nn_dst_idx_meta->filename, &nn_dst_idx_X,
	  &nn_dst_idx_nX, 0); cBYE(status);
  }
  //----------------------------------------------------------------

  if ( ( strcmp(src_fld_meta->fldtype, "float") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "double") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "int") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "bool") == 0 ) ||
       ( strcmp(src_fld_meta->fldtype, "long long") == 0 ) ) {
    status = xfer_nonstring(src_fld_meta, src_nR, src_fld_X, nn_src_fld_X, 
	dst_idx_meta, dst_nR, dst_idx_X, nn_dst_idx_X, dst_tbl, dst_fld);
    cBYE(status);
  }
  else if ( strcmp(src_fld_meta->fldtype, "char string") == 0 ) {
    status = xfer_string(dst_idx_meta, src_fld_X, sz_src_fld_X,
	    src_nR, dst_tbl, dst_fld, dst_idx_X, nn_dst_idx_X, dst_nR);
    cBYE(status);
  }
  else { go_BYE(-1); }
 BYE:
  rs_munmap(szX, n_szX);
  rs_munmap(nnX, n_nnX);
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(dst_idx_X, dst_idx_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  rs_munmap(nn_dst_idx_X, nn_dst_idx_nX);
  return(status);
}
