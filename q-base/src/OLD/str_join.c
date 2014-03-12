#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "del_fld.h"
#include "meta_globals.h"

/* str_join is used when the link fields are both strings. This is a
 * very ineffcient implemtantation that needs optimization badly */
//---------------------------------------------------------------
// START FUNC DECL
int 
str_join(
	 char *src_tbl,
	 char *src_lnk,
	 char *src_val,
	 char *dst_tbl,
	 char *dst_lnk,
	 char *dst_fld
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *src_val_X = NULL; size_t src_val_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;
  char *nn_src_lnk_X = NULL; size_t nn_src_lnk_nX = 0;
  char *sz_src_lnk_X = NULL; size_t sz_src_lnk_nX = 0;

  char *dst_val_X = NULL; size_t dst_val_nX = 0;
  char *nn_dst_val_X = NULL; size_t nn_dst_val_nX = 0;

  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  char *nn_dst_lnk_X = NULL; size_t nn_dst_lnk_nX = 0;
  char *sz_dst_lnk_X = NULL; size_t sz_dst_lnk_nX = 0;
  FLD_TYPE *src_val_meta = NULL, *src_lnk_meta = NULL;
  FLD_TYPE                       *dst_lnk_meta = NULL;
  long long src_nR, dst_nR;  // counts all rows 
  char str_meta_data[1024]; 
  bool is_any_null = false;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int dst_fld_id = INT_MIN; 
  int dst_lnk_id = INT_MIN, nn_dst_lnk_id = INT_MIN, sz_dst_lnk_id = INT_MIN;
  int src_lnk_id = INT_MIN, nn_src_lnk_id = INT_MIN, sz_src_lnk_id = INT_MIN;
  int src_val_id = INT_MIN;
  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  FLD_TYPE *nn_src_lnk_meta = NULL, *sz_src_lnk_meta = NULL;
  FLD_TYPE *nn_dst_lnk_meta = NULL, *sz_dst_lnk_meta = NULL;

  int *isrc_sz_ptr = NULL, *idst_sz_ptr = NULL;
  char *c_src_ptr = NULL, *c_dst_ptr = NULL;
  int *i_dst_val = NULL, *i_src_val = NULL;
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  //----------------------------------------------------------------
  // Get meta-data for all necessary fields 
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_lnk, &src_lnk_id); cBYE(status);
  chk_range(src_lnk_id, 0, g_n_fld);
  src_lnk_meta = &(g_fld[src_lnk_id]);

  status = is_fld(NULL, src_tbl_id, src_val, &src_val_id); cBYE(status);
  chk_range(src_val_id, 0, g_n_fld);
  src_val_meta = &(g_fld[src_val_id]);

  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbl[dst_tbl_id].nR;

  status = is_fld(NULL, dst_tbl_id, dst_lnk, &dst_lnk_id); cBYE(status);
  chk_range(dst_lnk_id, 0, g_n_fld);
  dst_lnk_meta = &(g_fld[dst_lnk_id]);

  //---------------------------------------------------------
  // Get meta data on auxiliary fields
  nn_src_lnk_id = src_lnk_meta->nn_fld_id;
  if ( nn_src_lnk_id >= 0 ) { 
    nn_src_lnk_meta = &(g_fld[nn_src_lnk_id]);
    status = rs_mmap(nn_src_lnk_meta->filename, &nn_src_lnk_X,
	&nn_src_lnk_nX, 0); cBYE(status);
  }
  nn_dst_lnk_id = dst_lnk_meta->nn_fld_id;
  if ( nn_dst_lnk_id >= 0 ) { 
    nn_dst_lnk_meta = &(g_fld[nn_dst_lnk_id]);
    status = rs_mmap(nn_dst_lnk_meta->filename, &nn_dst_lnk_X,
	&nn_dst_lnk_nX, 0); cBYE(status);
  }

  sz_src_lnk_id = src_lnk_meta->sz_fld_id;
  sz_src_lnk_meta = &(g_fld[sz_src_lnk_id]);
  status = rs_mmap(sz_src_lnk_meta->filename, &sz_src_lnk_X,
 	&sz_src_lnk_nX, 0); cBYE(status);

  sz_dst_lnk_id = dst_lnk_meta->sz_fld_id;
  sz_dst_lnk_meta = &(g_fld[sz_dst_lnk_id]);
  status = rs_mmap(sz_dst_lnk_meta->filename, &sz_dst_lnk_X,
 	&sz_dst_lnk_nX, 0); cBYE(status);

  isrc_sz_ptr = (int *)sz_src_lnk_X;
  idst_sz_ptr = (int *)sz_dst_lnk_X;
  //---------------------------------------------------------
  if ( ( strcmp(src_lnk_meta->fldtype, "char string") != 0 ) ||
       ( strcmp(dst_lnk_meta->fldtype, "char string") != 0 ) ) {
    go_BYE(-1);
  }
  if ( strcmp(src_val_meta->fldtype, "int") != 0 ) {
    go_BYE(-1);
  }
  //----------------------------------------------------------------
  // Get pointer access to all necessary fields
  status = rs_mmap(src_lnk_meta->filename, &src_lnk_X, &src_lnk_nX, 0); 
  cBYE(status);
  status = rs_mmap(src_val_meta->filename, &src_val_X, &src_val_nX, 0); 
  cBYE(status);
  status = rs_mmap(dst_lnk_meta->filename, &dst_lnk_X, &dst_lnk_nX, 0); 
  cBYE(status);
  //--------------------------------------------------------
  // Create output data files
  long long filesz = src_val_meta->n_sizeof * dst_nR;
  status = open_temp_file(&ofp, &opfile, 0); cBYE(status); 
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);

  long long nnfilesz = sizeof(char) * dst_nR;
  status = open_temp_file(&nn_ofp, &nn_opfile, nnfilesz); cBYE(status); 
  fclose_if_non_null(nn_ofp);
  status = mk_file(nn_opfile, nnfilesz); cBYE(status);

  status = rs_mmap(nn_opfile, &nn_dst_val_X, &nn_dst_val_nX, 1); cBYE(status);
  status = rs_mmap(opfile,    &dst_val_X,    &dst_val_nX,    1); cBYE(status);
  //--------------------------------------------------------
  //--------------------------------------------------------
  c_dst_ptr = (char *)dst_lnk_X;
  i_dst_val = (int *)dst_val_X;
  i_src_val = (int *)src_val_X;
  for ( long long i = 0; i < dst_nR; i++ ) { 
    i_dst_val[i] = 0;
    nn_dst_val_X[i] = FALSE;
    if ( ( nn_dst_lnk_X != NULL ) && ( nn_dst_lnk_X[i] == FALSE ) ) {
      nn_dst_val_X[i] = FALSE;
    }
    else {
      c_src_ptr = (char *)src_lnk_X;
      for ( long long j = 0; j < src_nR; j++ ) { 
        if ( ( nn_src_lnk_X != NULL ) && ( nn_src_lnk_X[j] == FALSE ) ) {
	  /* No compariso to be made */
	}
	else {
	if ( strcmp(c_dst_ptr, c_src_ptr) == 0 ) {
	  i_dst_val[i] = i_src_val[j];
          nn_dst_val_X[i] = TRUE;
	  break;
	}
	}
        c_src_ptr += isrc_sz_ptr[j];
      }
    }
    c_dst_ptr += idst_sz_ptr[i];
  }
  for ( long long i = 0; i < dst_nR; i++ ) { 
    if ( nn_dst_val_X[i] == FALSE ) { 
      is_any_null = true;
      break;
    }
  }
  //--------------------------------------------------------
  // Add output field to meta data 
  sprintf(str_meta_data,"filename=%s:n_sizeof=%d:fldtype=%s", opfile,
	    src_val_meta->n_sizeof, src_val_meta->fldtype);
  status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id);
  cBYE(status);
  if ( is_any_null == true ) {
    status = add_aux_fld(dst_tbl, dst_fld, nn_opfile, "nn", &dst_fld_id);
    cBYE(status);
  }
  else {
    unlink(nn_opfile);
    free_if_non_null(nn_opfile);
  }
 BYE:
  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_val_X, dst_val_nX);
  rs_munmap(nn_dst_val_X, nn_dst_val_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);
  free_if_non_null(nn_opfile);
  free_if_non_null(opfile);
  return(status);
}
