/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "xfer.h"
#include "add_fld.h"
#include "get_meta.h"
#include "set_meta.h"
#include "add_aux_fld.h"
#include "meta_globals.h"

#include "xfer_I1_I4.h"
#include "xfer_I2_I4.h"
#include "xfer_I4_I4.h"
#include "xfer_I8_I4.h"
#include "xfer_F4_I4.h"
#include "xfer_F8_I4.h"

extern char *g_data_dir;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
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
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, dst_idx_meta, dst_fld_rec; 
  FLD_REC_TYPE nn_src_fld_rec, nn_dst_idx_meta, nn_dst_fld_rec; 
  char opfile[MAX_LEN_FILE_NAME+1];
  char nn_opfile[MAX_LEN_FILE_NAME+1];

  long long src_nR, dst_nR;

  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *dst_fld_X = NULL; size_t dst_fld_nX = 0;
  char *dst_idx_X = NULL; size_t dst_idx_nX = 0;

  char *nn_src_fld_X = NULL; size_t nn_src_fld_nX = 0;
  char *nn_dst_idx_X = NULL; size_t nn_dst_idx_nX = 0;
  char *nn_dst_fld_X = NULL; size_t nn_dst_fld_nX = 0;

  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;
  int dst_idx_id = INT_MIN, nn_dst_idx_id = INT_MIN;
  int dst_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;

  //----------------------------------------------------------------
  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  zero_string(nn_opfile, MAX_LEN_FILE_NAME+1);
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) )  { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_idx == NULL ) || ( *dst_idx == '\0' ) )  { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) )  { go_BYE(-1); }
  if ( strcmp(dst_idx, dst_fld) == 0 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
      &nn_src_fld_id, &nn_src_fld_rec);
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);

  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, false); cBYE(status);

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;

  status = is_fld(NULL, dst_tbl_id, dst_idx, &dst_idx_id, &dst_idx_meta, 
      &nn_dst_idx_id, &nn_dst_idx_meta);
  cBYE(status);
  chk_range(dst_idx_id, 0, g_n_fld);
  status = get_data(dst_idx_meta, &dst_idx_X, &dst_idx_nX, false); cBYE(status);

  bool is_nn_needed = false;
  if ( nn_src_fld_id >= 0 ) { 
    status = get_data(nn_src_fld_rec, &nn_src_fld_X, &nn_src_fld_nX, false); 
    cBYE(status);
    is_nn_needed = true;
  }
  if ( nn_dst_idx_id >= 0 ) { 
    status = get_data(nn_dst_idx_meta, &nn_dst_idx_X, &nn_dst_idx_nX, false); 
    cBYE(status);
    is_nn_needed = true;
  }
  //----------------------------------------------------------------
  // allocate space for output
  int fldsz = INT_MAX, ddir_id = INT_MAX, nn_ddir_id = INT_MAX;
  status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);

  status = mk_temp_file(opfile, (dst_nR * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &dst_fld_X, &dst_fld_nX, true); cBYE(status);

  if ( is_nn_needed ) { 
    status = mk_temp_file(nn_opfile, (dst_nR * sizeof(char)), &nn_ddir_id); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_opfile, &nn_dst_fld_X, &nn_dst_fld_nX, true); cBYE(status);
  }
  

  //----------------------------------------------------------------
  switch ( src_fld_rec.fldtype ) { 
#include "incl_xfer_I1.c"
#include "incl_xfer_I2.c"
#include "incl_xfer_I4.c"
#include "incl_xfer_I8.c"
#include "incl_xfer_F4.c"
#include "incl_xfer_F8.c"
    default : 
      go_BYE(-1);
      break;
  }
  status = add_fld(dst_tbl_id, dst_fld, ddir_id, opfile, src_fld_rec.fldtype,
      -1, &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  // Note that is_nn_needed is overly pessimistic in its assessment of
  // whether an nn field is needed. It may not be needed. We should
  // tighten this up at some point in time TODO P2
  if ( is_nn_needed ) { 
    status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_fld_id, nn_ddir_id, 
	nn_opfile, "nn", &nn_dst_fld_id, &nn_dst_fld_rec);
  }
  char strbuf[32]; 
  status = int_get_meta(src_tbl_id, src_fld_id, "dict_tbl_id", strbuf);
  cBYE(status);
  char *endptr; 
  int dict_tbl_id = strtoll(strbuf, &endptr, 10); 
  if ( dict_tbl_id > 0 ) {
    status = int_set_meta(dst_tbl_id, dst_fld_id, "dict_tbl_id", strbuf);
    cBYE(status);
  }
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(dst_idx_X, dst_idx_nX);
  rs_munmap(dst_fld_X, dst_fld_nX);
  rs_munmap(nn_src_fld_X, nn_src_fld_nX);
  rs_munmap(nn_dst_idx_X, nn_dst_idx_nX);
  rs_munmap(nn_dst_fld_X, nn_dst_fld_nX);
  return(status);
}
