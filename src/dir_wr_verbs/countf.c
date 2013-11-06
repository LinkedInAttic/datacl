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
/* READ */
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "set_meta.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "countf.h"
#include "countf_I1.h"
#include "countf_I2.h"
#include "countf_I4.h"
#include "countf_I8.h"
#include "countf_nn_I1.h"
#include "countf_nn_I2.h"
#include "countf_nn_I4.h"
#include "countf_nn_I8.h"
#include "assign_I4.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
countf(
       char *src_tbl,
       char *src_fld,
       char *fk_dst,
       char *cfld,
       char *dst_tbl,
       char *cnt_fld
       )
// STOP FUNC DECL
{
  int status = 0;
  char *fk_dst_X = NULL;    size_t fk_dst_nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *cfld_X = NULL; size_t cfld_nX = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;


  TBL_REC_TYPE src_tbl_rec; int src_tbl_id = -1; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id = -1; 

  FLD_REC_TYPE fk_dst_rec; int fk_dst_id = -1; 
  FLD_REC_TYPE nn_fk_dst_rec; int nn_fk_dst_id; 

  FLD_REC_TYPE src_fld_rec; int src_fld_id = -1; 
  FLD_REC_TYPE nn_src_fld_rec; int nn_src_fld_id; 

  FLD_REC_TYPE cfld_rec; int cfld_id = -1;
  FLD_REC_TYPE nn_cfld_rec; int nn_cfld_id;

  FLD_REC_TYPE cnt_fld_rec; int cnt_fld_id; 

  char opfile[MAX_LEN_FILE_NAME+1];

  long long src_nR, dst_nR;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fk_dst == NULL )  || ( *fk_dst == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbls[dst_tbl_id].nR;
  if ( dst_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, fk_dst, &fk_dst_id, &fk_dst_rec, 
		  &nn_fk_dst_id, &nn_fk_dst_rec); 
  cBYE(status);
  chk_range(fk_dst_id, 0, g_n_fld);
  status = get_data(fk_dst_rec, &fk_dst_X, &fk_dst_nX, 0); cBYE(status);
  if ( nn_fk_dst_id >= 0 ) { go_BYE(-1);  }

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); 
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  status = get_data(src_fld_rec, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  /* if src_fld has a nn field, then cannot have a cfld */
  if ( nn_src_fld_id >= 0 ) { 
    if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { go_BYE(-1);  }
    status = get_data(nn_src_fld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }

  if ( ( src_fld_rec.fldtype != I4 ) && ( src_fld_rec.fldtype != I8 ) ) {
    go_BYE(-1); 
  }
  int       *I4_src_ptr = (int       *)src_fld_X;

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, cfld, &cfld_id, &cfld_rec, 
		    &nn_cfld_id, &nn_cfld_rec); 
    if ( cfld_id < 0 ) { go_BYE(-1); }
    if ( cfld_rec.fldtype != I1 ) { go_BYE(-1); }
    if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
    status = get_data(cfld_rec, &cfld_X, &cfld_nX, 0); cBYE(status);
  }
  //------------------------------------------------------
  FLD_TYPE cnt_fld_type = I8;
  int ddir_id = INT_MAX, cnt_fldsz = -1; long long filesz = 0;
  status = get_fld_sz(cnt_fld_type, &cnt_fldsz); cBYE(status);
  filesz = dst_nR * cnt_fldsz;
  status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &op_X, &op_nX, true); cBYE(status);
  long long *cntI8 = (long long *)op_X;
  for ( int i = 0; i < dst_nR ; i++ ) {
    cntI8[i] = 0;
  }
  //------------------------------------------------------
  if ( dst_nR > INT_MAX ) { go_BYE(-1); } /* required by count_In */
  // Initialize counters to 0 
  for ( int i = 0; i < dst_nR; i++ ) { cntI8[i] = 0; }

  if ( cfld_id >= 0 ) {
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = countf_nn_I1((char *)fk_dst_X, I4_src_ptr, src_nR, 
	  cfld_X, cntI8, dst_nR); 
      cBYE(status); 
      break;
    case I2 : 
      status = countf_nn_I2((short *)fk_dst_X, I4_src_ptr, src_nR, 
	  cfld_X, cntI8, dst_nR); 
      cBYE(status); 
      break;
    case I4 : 
      status = countf_nn_I4((int *)fk_dst_X, I4_src_ptr, src_nR, 
	  cfld_X, cntI8, dst_nR); 
      cBYE(status); 
      break;
    case I8 : 
      status = countf_nn_I8((long long *)fk_dst_X, I4_src_ptr, src_nR, 
	  cfld_X, cntI8, dst_nR); 
      cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else {
    switch ( fk_dst_rec.fldtype ) {
    case I1 : 
      status = countf_I1((char *)fk_dst_X, I4_src_ptr, src_nR, 
	  cntI8, dst_nR); cBYE(status);
      break;
    case I2 : 
      status = countf_I2((short *)fk_dst_X, I4_src_ptr, src_nR, 
	  cntI8, dst_nR); cBYE(status); 
      break;
    case I4 : 
      status = countf_I4((int *)fk_dst_X, I4_src_ptr, src_nR, 
	  cntI8, dst_nR); cBYE(status); 
      break;
    case I8 : 
      status = countf_I8((long long *)fk_dst_X, I4_src_ptr, src_nR, 
	  cntI8, dst_nR); cBYE(status); 
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }

  rs_munmap(op_X, op_nX);
  status = add_fld(dst_tbl_id, cnt_fld, ddir_id, opfile, cnt_fld_type, -1, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
 BYE:
  rs_munmap(fk_dst_X, fk_dst_nX);
  rs_munmap(op_X, op_nX);
  rs_munmap(cfld_X, cfld_nX);
  rs_munmap(src_fld, src_fld_nX);
  return(status);
}
