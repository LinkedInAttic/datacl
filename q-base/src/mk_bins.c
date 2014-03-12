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
#include "f_to_s.h"
#include "s_to_f.h"
#include "mk_bins.h"
#include "bindmp.h"
#include "bin_search_I4.h"
#include "sortbindmp.h"
#include "assign_I4.h"
#include "assign_I8.h"
#include "meta_globals.h"

// last review 9/10/2013
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


  int fldsz; 
  int lb_ddir_id  = -1, lb_fileno  = -1;
  int ub_ddir_id  = -1, ub_fileno  = -1;
  int cnt_ddir_id = -1, cnt_fileno = -1;
  //----------------------------------------------------------------
  
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL )  || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  zero_string(buffer, BUFLEN);
  status = stoI8(str_dst_nR, &dst_nR); cBYE(status);
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
  status = int_get_meta(src_tbl_id, src_fld_id, "srttype", buffer); cBYE(status);
  if ( strcmp(buffer, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_fld, src_tbl);
    go_BYE(-1);
  }
  //--------------------------------------------------------
  // Create output space. May need to be truncated later on
  status = get_fld_sz(src_fld_rec.fldtype, &fldsz); cBYE(status);
  status = mk_temp_file((dst_nR * fldsz), &lb_ddir_id, &lb_fileno);cBYE(status);
  status = q_mmap(lb_ddir_id, lb_fileno, &lb_X, &lb_nX, true); cBYE(status);
  status = mk_temp_file((dst_nR * fldsz), &ub_ddir_id, &ub_fileno);cBYE(status);
  status = q_mmap(ub_ddir_id, ub_fileno, &ub_X, &ub_nX, true); cBYE(status);
  status = mk_temp_file((dst_nR * sizeof(long long)), &cnt_ddir_id, &cnt_fileno); 
  cBYE(status);
  status = q_mmap(cnt_ddir_id, cnt_fileno, &cnt_X, &cnt_nX, true); cBYE(status);
  //------------------------------------------------------
  // Now we can finally get to work!
  int *I4lb  = (int *)lb_X;
  int *I4ub  = (int *)ub_X;
  assign_const_I4(I4lb, dst_nR, INT_MAX); // initialize with "bad" values
  assign_const_I4(I4ub, dst_nR, INT_MIN); // initialize with "bad" values
  long long *I8cnt = (long long *)cnt_X;
  int *items = (int *)X;
  assign_const_I8(I8cnt, dst_nR, 0); // initialize to 0
  int idx = 0;
  int num_bins_wanted = dst_nR;
  int num_bins_created = 0;
  long long num_items_left = src_nR;
  int currval;
  for ( ; ; ) {
    if ( num_bins_created >= num_bins_wanted ) { go_BYE(-1); }
    long long stopidx_1, stopidx_2, pos;
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

    int bin_size = num_items_left / (num_bins_wanted - num_bins_created);
    if ( bin_size == 0 ) { bin_size++; }
    stopidx_1 = idx + bin_size;
    currval = items[idx];
    I4lb[num_bins_created] = currval;
    if ( stopidx_1 >= src_nR ) {
      /* close out the last bin */
      I4ub[num_bins_created] = items[src_nR-1];
      if ( num_bins_created > 0 ) {
        if (I4lb[num_bins_created-1] == I4lb[num_bins_created]) {go_BYE(-1);}
      }
      I8cnt[num_bins_created] = src_nR - idx ; 
      num_bins_created++;
      break;
    }
    int nextval = items[stopidx_1];
    I4ub[num_bins_created] = nextval;
    // do binary search to find the next location with same val as nextval 
    int *I4ptr = items + idx;
    status=bin_search_I4(I4ptr, num_items_left, nextval, &pos, "highest"); 
    if ( pos < 0 ) { go_BYE(-1); }
    stopidx_2 = pos + idx;
    if ( stopidx_2 < stopidx_1 ) { go_BYE(-1); }
    I4ub[num_bins_created]  = nextval;
    I8cnt[num_bins_created] = stopidx_2 - idx + 1;
    idx = stopidx_2 + 1;

    num_items_left -= I8cnt[num_bins_created];
    if ( num_items_left + idx != src_nR ) { go_BYE(-1); }
    num_bins_created++;
    if ( idx == src_nR ) { break; } // consumed all input 
    if ( idx > src_nR ) { go_BYE(-1); }
  }
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
  q_trunc(lb_ddir_id, lb_fileno,   (dst_nR * fldsz));
  q_trunc(ub_ddir_id, ub_fileno,   (dst_nR * fldsz));
  q_trunc(cnt_ddir_id, cnt_fileno, (dst_nR * sizeof(long long)));
  //-----------------------------------------------------------
  status = del_tbl(dst_tbl, -1); cBYE(status);
  sprintf(buffer, "%lld", dst_nR);
  status = add_tbl(dst_tbl, buffer, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  //-----------------------------------------------------------
  // Add lb field to dst table 
  zero_fld_rec(&out_lb_fld_rec); out_lb_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_fld(dst_tbl_id, "lb", lb_ddir_id, lb_fileno, 
      &out_lb_fld_id, &out_lb_fld_rec);
  cBYE(status);

  // Add ub field to dst table 
  zero_fld_rec(&out_ub_fld_rec); out_ub_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_fld(dst_tbl_id, "ub", ub_ddir_id, ub_fileno, 
      &out_ub_fld_id, &out_ub_fld_rec);
  cBYE(status);

  // Add count field to dst table 
  zero_fld_rec(&out_cnt_fld_rec); out_cnt_fld_rec.fldtype = I8;
  status = add_fld(dst_tbl_id, "cnt", cnt_ddir_id, cnt_fileno, 
      &out_cnt_fld_id, &out_cnt_fld_rec);
  cBYE(status);

  // Since src_fld is sorted ascending, so is lb/ub in dst_tbl
  status = int_set_meta(dst_tbl_id, out_lb_fld_id, "srttype", "ascending", true);
  cBYE(status);
  status = int_set_meta(dst_tbl_id, out_ub_fld_id, "srttype", "ascending", true);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(X, nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  rs_munmap(cnt_X, cnt_nX);
  return(status);
}
