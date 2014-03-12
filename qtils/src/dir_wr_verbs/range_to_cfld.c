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
#include "add_fld.h"
#include "range_to_cfld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_nn_fld.h"
#include "open_temp_file.h"
#include "mk_temp_file.h"
#include "del_fld.h"
#include "fop.h"
#include "set_meta.h"
#include "assign_I1.h"
#include "assign_I8.h"
#include "meta_globals.h"

// START FUNC DECL
int 
range_to_cfld(
	      char *rng_tbl,
	      char *rng_lb_fld,
	      char *rng_ub_fld,
	      char *tbl,
	      char *src_fld,
	      char *dst_fld,
	      char *str_dst_fldtype
	      )
// STOP FUNC DECL
{
  int status = 0;
  char *idx_X = NULL; size_t idx_nX = 0;
  char *dst_X = NULL; size_t dst_nX = 0;
  char *src_X = NULL; size_t src_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;

  TBL_REC_TYPE tbl_rec, rng_tbl_rec;
  FLD_REC_TYPE rng_lb_fld_rec, nn_rng_lb_fld_rec; 
  FLD_REC_TYPE rng_ub_fld_rec, nn_rng_ub_fld_rec;
  FLD_REC_TYPE dst_fld_rec;
  FLD_REC_TYPE src_fld_rec,    nn_src_fld_rec;
  FLD_TYPE dst_fldtype; 

  long long nR = LLONG_MIN, rng_nR = LLONG_MIN;
  int tbl_id = INT_MIN, rng_tbl_id = INT_MIN;
  int rng_lb_fld_id = INT_MIN, nn_rng_lb_fld_id = INT_MIN;
  int dst_fld_id = INT_MIN; 
  int src_fld_id = INT_MIN,    nn_src_fld_id = INT_MIN;
  int rng_ub_fld_id = INT_MIN, nn_rng_ub_fld_id = INT_MIN;

  char opfile[MAX_LEN_FILE_NAME+1];
  char idxfile[MAX_LEN_FILE_NAME+1];
  //----------------------------------------------------------------
  zero_string(opfile, MAX_LEN_FILE_NAME+1);
  zero_string(idxfile, MAX_LEN_FILE_NAME+1);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(dst_fld, src_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(rng_tbl, tbl)     == 0 ) { go_BYE(-1); }
  if ( ( rng_tbl == NULL ) || ( *rng_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( rng_lb_fld == NULL ) || ( *rng_lb_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( rng_ub_fld == NULL ) || ( *rng_ub_fld == '\0' ) ) { go_BYE(-1); }
  //----------------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;

  status = is_tbl(rng_tbl, &rng_tbl_id, &rng_tbl_rec); cBYE(status);
  chk_range(rng_tbl_id, 0, g_n_tbl);
  rng_nR = g_tbls[rng_tbl_id].nR;

  // Decide whether output fldtype is B or I1 and size file accordingly
  long long filesz; int ddir_id;
  if ( strcmp(str_dst_fldtype, "B" ) == 0 ) { 
    dst_fldtype = B;
    status = get_file_size_B(nR, &filesz); cBYE(status);
  }
  else if ( strcmp(str_dst_fldtype, "I1" ) == 0 ) { 
    dst_fldtype = I1;
    filesz = nR * sizeof(char);  
  }
  else { dst_fldtype = xunknown; go_BYE(-1); }

  // Create space for output
  status = mk_temp_file(opfile, filesz, &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &dst_X, &dst_nX, true); cBYE(status);

  // rng_lb_fld
  status = is_fld(NULL, rng_tbl_id, rng_lb_fld, &rng_lb_fld_id, 
		  &rng_lb_fld_rec, &nn_rng_lb_fld_id, &nn_rng_lb_fld_rec); cBYE(status);
  chk_range(rng_lb_fld_id, 0, g_n_fld);
  if ( nn_rng_lb_fld_id >= 0 ) { go_BYE(-1); }
  status = get_data(rng_lb_fld_rec, &lb_X, &lb_nX, false); cBYE(status);
  if ( rng_lb_fld_rec.fldtype != I8 ) { go_BYE(-1); }
  long long *lbI8 = (long long *)lb_X;

  // rng_ub_fld
  status = is_fld(NULL, rng_tbl_id, rng_ub_fld, &rng_ub_fld_id, 
		  &rng_ub_fld_rec, &nn_rng_ub_fld_id, &nn_rng_ub_fld_rec); cBYE(status);
  chk_range(rng_ub_fld_id, 0, g_n_fld);
  if ( nn_rng_ub_fld_id >= 0 ) { go_BYE(-1); }
  if ( rng_ub_fld_rec.fldtype != I8 ) { go_BYE(-1); }
  status = get_data(rng_ub_fld_rec, &ub_X, &ub_nX, false); cBYE(status);
  long long *ubI8 = (long long *)ub_X;

  // src__fld
  status = is_fld(NULL, tbl_id, src_fld, &src_fld_id, 
		  &src_fld_rec, &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  if ( nn_src_fld_id >= 0 ) { go_BYE(-1); }
  status = get_data(src_fld_rec, &src_X, &src_nX, false); cBYE(status);

  // Set output to 0 as default. Note that we use dst_nX, the number of
  // bytes in the output file and not nR (the number of rows)
  long long block_size, num_blocks;
  char *cptr = NULL; 
  block_size = 4096;
  num_blocks = dst_nX / block_size;
  for ( long long i = 0; i < num_blocks; i++ ) { 
    char *lcl_cptr = dst_X + (i*block_size);
    assign_const_I1(lcl_cptr, block_size, 0);
  }
  cptr = dst_X + (num_blocks * block_size);
  assign_const_I1(cptr, (dst_nX- (num_blocks*block_size)), 0);
  //------------------------------------------------------
  // make temporary storage for the indexes 
  // TODO P3: Currently we use I8 for indexes even when I4 suffices
  long long idx_nR = 0;
  for ( int i = 0; i < rng_nR; i++ ) { 
    long long lb = lbI8[i];
    long long ub = ubI8[i];
    if ( ( lb < 0 ) || ( lb > nR ) )  { go_BYE(-1); }
    if ( ( ub < 0 ) || ( ub > nR ) )  {  go_BYE(-1); }
    if ( lb >= ub ) { go_BYE(-1); }
    idx_nR += (ub - lb);
  }
  int idx_ddir_id;
  filesz = idx_nR * sizeof(long long);
  status = mk_temp_file(idxfile, filesz, &idx_ddir_id); cBYE(status);
  status = q_mmap(idx_ddir_id, idxfile, &idx_X, &idx_nX, true); cBYE(status);
  long long *idxI8 = (long long *)idx_X;

  // P3 TODO: Determine whether there is conflict with Cilk and sharing.
  long long idxctr = 0; long long lb, ub;
  switch ( src_fld_rec.fldtype ) {
  case I4 : 
    for ( int i = 0; i < rng_nR; i++ ) { 
      int *srcI4 = (int *)src_X;
      lb = lbI8[i]; 
      ub = ubI8[i];
      for ( long long j = lb; j < ub; j++ ) {
	int idx = srcI4[j];
	idxI8[idxctr + (j-lb)] = idx;
      }
      cBYE(status);
      idxctr += (ub - lb);
    }
    break;
  case I8 : 
    for ( int i = 0; i < rng_nR; i++ ) { 
      long long *srcI8 = (long long *)src_X;
      lb = lbI8[i]; 
      ub = ubI8[i];
      for ( long long j = lb; j < ub; j++ ) { 
	long long idx = srcI8[j];
	idxI8[idxctr+(j-lb)] = idx;
      }
      cBYE(status);
      idxctr += (ub - lb);
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }


  switch ( dst_fldtype ) { 
  case I1 : 
    for ( long long i = 0; i < idx_nR; i++ ) { 
      long long idx = idxI8[i];
      dst_X[idx] = TRUE;
    }
    break;
  case B : 
    for ( long long i = 0; i < idx_nR; i++ ) { 
      long long idx = idxI8[i]; unsigned char uchar;
      bool optimize = true; long long word_idx; int bit_idx; 
      if ( optimize ) { 
	word_idx = (unsigned int)idx >> 3; /* 2^3 = NUM_BITS_IN_I1 */
	bit_idx = idx - ( word_idx << 3 );
      }
      else {
	word_idx = idx / NUM_BITS_IN_I1;
	bit_idx  = idx % NUM_BITS_IN_I1;
      }
      uchar = 1 << bit_idx;
      dst_X[word_idx] |= uchar;
    }
    break;
  default :
    go_BYE(-1); 
    break;
  }
  status = add_fld(tbl_id, dst_fld, ddir_id, opfile, dst_fldtype, 
      -1, &dst_fld_id, &dst_fld_rec); cBYE(status);
  char strbuf[32];
  sprintf(strbuf, "%lld", idx_nR);
  status = int_set_meta(tbl_id, dst_fld_id, "cnt", strbuf); 
  cBYE(status);
 BYE:
  if ( *idxfile != '\0' ) { q_delete(idx_ddir_id, idxfile); }
  rs_munmap(dst_X, dst_nX);
  rs_munmap(src_X, src_nX);
  rs_munmap(idx_X, idx_nX);
  rs_munmap(lb_X,  lb_nX);
  rs_munmap(ub_X,  ub_nX);
  return(status);
}
