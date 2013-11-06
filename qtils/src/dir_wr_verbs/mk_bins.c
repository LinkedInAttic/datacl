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
#include "aux_meta.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_temp_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "set_meta.h"
#include "get_meta.h"
#include "ext_f_to_s.h"
#include "ext_s_to_f.h"
#include "open_temp_file.h"
#include "mk_bins.h"
#include "bindmp.h"
#include "bin_search_I4.h"

#include "meta_globals.h"

extern char *g_data_dir;
extern char *g_cwd;
//---------------------------------------------------------------
// START FUNC DECL
int 
mk_bins(
	   char *src_tbl,
	   char *src_fld,
	   char *dst_tbl,
	   char *str_dst_nR
	   )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;

  FLD_REC_TYPE src_fld_rec, nn_src_fld_rec;

  FLD_REC_TYPE out_lb_fld_rec, out_ub_fld_rec, out_cnt_fld_rec;

  long long src_nR = LLONG_MIN, dst_nR = LLONG_MIN; 

  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;

  int src_fld_id = INT_MIN, nn_src_fld_id = INT_MIN;

  int out_lb_fld_id, out_ub_fld_id, out_cnt_fld_id;

#define BUFLEN 32
  char buffer[BUFLEN];

  char *X = NULL; size_t nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;

  char lb_opfile[MAX_LEN_FILE_NAME+1];
  char ub_opfile[MAX_LEN_FILE_NAME+1];
  char cnt_opfile[MAX_LEN_FILE_NAME+1];

  int fldsz, lb_ddir_id = INT_MAX, ub_ddir_id = INT_MAX, cnt_ddir_id = INT_MAX;
  //----------------------------------------------------------------
  
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL )  || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(buffer, BUFLEN);
  zero_string(lb_opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(ub_opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(cnt_opfile, (MAX_LEN_FILE_NAME+1));
  char *endptr;
  dst_nR = strtoll(str_dst_nR, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( dst_nR <= 1 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  if ( dst_nR >= src_nR )  { dst_nR = src_nR; }
  //--------------------------------------------------------
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
      &nn_src_fld_id, &nn_src_fld_rec); 
  cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  status = get_data(src_fld_rec, &X, &nX, false); cBYE(status);
  if ( nn_src_fld_id >= 0 ) { go_BYE(-1); }
  if ( src_fld_rec.fldtype != I4 ) { go_BYE(-1); }
  //--------------------------------------------------------
  zero_string(buffer, BUFLEN);
  status = int_get_meta(src_tbl_id, src_fld_id, "sort_type", buffer); cBYE(status);
  if ( strcmp(buffer, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  // Create output space. May need to be truncated later on
  status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);

  status = mk_temp_file(lb_opfile, (dst_nR * fldsz), &lb_ddir_id); cBYE(status);
  status = q_mmap(lb_ddir_id, lb_opfile, &lb_X, &lb_nX, true); cBYE(status);

  status = mk_temp_file(ub_opfile, (dst_nR * fldsz), &ub_ddir_id); cBYE(status);
  status = q_mmap(ub_ddir_id, ub_opfile, &ub_X, &ub_nX, true); cBYE(status);

  status = mk_temp_file(cnt_opfile, (dst_nR * sizeof(long long)), &cnt_ddir_id); cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_opfile, &cnt_X, &cnt_nX, true); cBYE(status);
  //------------------------------------------------------
  // Now we can finally get to work!
  int *I4lb  = (int *)lb_X;
  int *I4ub  = (int *)ub_X;
  for ( int i = 0; i  < dst_nR; i++ ) { 
    I4lb[i] = INT_MAX;
    I4ub[i] = INT_MIN;
  }
  long long *I8cnt = (long long *)cnt_X;
  int *items = (int *)X;
  for ( int i = 0; i < dst_nR; i++ ) { 
    I8cnt[i] = 0;
  }
  int idx = 0;
  int num_bins_wanted = dst_nR;
  int num_bins_created = 0;
  int num_items_left = src_nR;
  int currval;
  for ( ; ; ) {
    long long stopidx_1, stopidx_2, stopidx_3, pos;
    int num_items_consumed = src_nR - num_items_left;
#ifdef DEBUG
    long long chk_cnt = 0;
    for ( int i = 0; i < dst_nR; i++ ) {
      chk_cnt += I8cnt[i];
    }
    if ( chk_cnt != num_items_consumed ) {
      go_BYE(-1);
    }
#endif

    if ( num_bins_created >= num_bins_wanted ) { go_BYE(-1); }
    int bin_size = num_items_left / (num_bins_wanted - num_bins_created);
    if ( bin_size == 0 ) { bin_size++; }
    stopidx_1 = idx + bin_size;
    if ( stopidx_1 >= src_nR ) {
      /* close out the last bin */
      currval = items[idx];
      I4lb[num_bins_created] = currval;
      if ( I4lb[num_bins_created-1] == currval ) { go_BYE(-1); }
      I4ub[num_bins_created] = items[src_nR-1];
      I8cnt[num_bins_created] = src_nR - idx ; 
      num_bins_created++;
      break;
    }
    currval = items[idx];
    I4lb[num_bins_created] = currval;
    // do binary search to find the next location with same val as current val
    status=bin_search_I4(items+idx, num_items_left, currval, &pos, "highest"); 
    if ( pos < 0 ) { go_BYE(-1); }
    stopidx_2 = pos + idx;
    if ( stopidx_2 > stopidx_1 ) {
      /* This means that the number of items with value "currval"
       * exceeds the bin size. So, in this case lb=ub */
      I4ub[num_bins_created] = currval;
      I8cnt[num_bins_created] = stopidx_2 - idx + 1;
      idx = stopidx_2 + 1;
    }
    else {
      int nextval = items[stopidx_1];
      I4ub[num_bins_created] = nextval;

      status = bin_search_I4(items+idx, num_items_left, nextval, &pos, "highest"); 
      if ( pos < 0 ) { 
	go_BYE(-1); 
      }
      stopidx_3  = pos + idx;
      if ( ( stopidx_3 - stopidx_1 + 1 ) <= 0 ) {
	printf("HI\n");
      }
      I8cnt[num_bins_created] = stopidx_3 - idx + 1;
      idx = stopidx_3 + 1;
    }
    num_items_left -= I8cnt[num_bins_created];
    if ( num_items_left + idx != src_nR ) {
      go_BYE(-1); 
    }
    num_bins_created++;
    if ( idx == src_nR ) { break; } // consumed all input 
    if ( idx > src_nR ) { go_BYE(-1); }
  }
  if ( num_bins_created > dst_nR ) { go_BYE(-1); }
  dst_nR = num_bins_created;
#ifdef DEBUG
  long long chk_cnt = 0;
  for ( int i = 0; i < dst_nR; i++ ) { 
    if ( I4lb[i] > I4ub[i] ) { 
      go_BYE(-1); 
    }
    if ( i > 0 ) { 
      if ( I4lb[i] <= I4lb[i-1] ) {
	go_BYE(-1);
      }
    }
    // TODO P2: Is this check correct? if ( I8cnt[i] <= 0 ) { go_BYE(-1); }
    chk_cnt += I8cnt[i];
  }
  if ( chk_cnt != src_nR ) { go_BYE(-1); }
#endif
  //------------------------------------------------------
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  rs_munmap(cnt_X, cnt_nX);
  // truncate to proper size
  q_trunc(lb_ddir_id, lb_opfile,  (dst_nR * fldsz));
  q_trunc(ub_ddir_id, ub_opfile,  (dst_nR * fldsz));
  q_trunc(cnt_ddir_id, cnt_opfile, (dst_nR * sizeof(long long)));
  //-----------------------------------------------------------
  status = del_tbl(dst_tbl, -1); cBYE(status);
  sprintf(buffer, "%lld", dst_nR);
  status = add_tbl(dst_tbl, buffer, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  //-----------------------------------------------------------
  status = add_fld(dst_tbl_id, "lb", lb_ddir_id, lb_opfile, 
      src_fld_rec.fldtype,  -1, &out_lb_fld_id, &out_lb_fld_rec);
  cBYE(status);

  status = add_fld(dst_tbl_id, "ub", ub_ddir_id, ub_opfile, 
      src_fld_rec.fldtype,  -1, &out_ub_fld_id, &out_ub_fld_rec);
  cBYE(status);

  // Add count field to dst table 
  status = add_fld(dst_tbl_id, "cnt", cnt_ddir_id, cnt_opfile, I8, 
      -1, &out_cnt_fld_id, &out_cnt_fld_rec);
  cBYE(status);

  // Since src_fld is sorted ascending, so is dst_fld 
  status = int_set_meta(dst_tbl_id, out_lb_fld_id, "sort_type", "ascending");
  status = int_set_meta(dst_tbl_id, out_ub_fld_id, "sort_type", "ascending");
  //-----------------------------------------------------------
 BYE:
  rs_munmap(X, nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  rs_munmap(cnt_X, cnt_nX);
  return(status);
}
