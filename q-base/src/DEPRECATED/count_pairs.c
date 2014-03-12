#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/time.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"

#include "uniq_int.h"
#include "uniq_longlong.h"
#include "uniq_alldef_int.h"
#include "uniq_alldef_longlong.h"
#include "uniq_cnt_alldef_i_i.h"
#include "uniq_cnt_alldef_ll_ll.h"
#include "uniq_cnt_alldef_ll_dbl.h"

/* TODO: Document the fact that we do not count 0/0 combinations */
//---------------------------------------------------------------
// START FUNC DECL
int 
count_pairs(
	   char *src_tbl,
	   char *grp_fld,
	   char *key_fld,
	   char *cnt_fld,
	   char *dst_tbl,
	   char *key1,
	   char *key2,
	   char *cnt1,
	   char *cnt2,
	   char *str_subsample
	   )
// STOP FUNC DECL
{
  int status = 0;

  char *grp_fld_X = NULL; size_t grp_fld_nX = 0;
  char *key_fld_X = NULL; size_t key_fld_nX = 0;
  char *cnt_fld_X = NULL; size_t cnt_fld_nX = 0;

  FLD_TYPE *grp_fld_meta = NULL; 
  FLD_TYPE *key_fld_meta = NULL;
  FLD_TYPE *cnt_fld_meta = NULL;

  long long src_nR; 
  long long dst_nR;

  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int grp_fld_id = INT_MIN, nn_grp_fld_id = INT_MIN;
  int key_fld_id = INT_MIN, nn_key_fld_id = INT_MIN;
  int cnt_fld_id = INT_MIN, nn_cnt_fld_id = INT_MIN;

  FILE *key1fp = NULL, *key2fp = NULL, *cnt1fp = NULL, *cnt2fp = NULL;
  char *key1fname = NULL, *key2fname = NULL;
  char *cnt1fname = NULL, *cnt2fname = NULL;

  struct timeval *Tps = NULL, *Tpf = NULL;

  char buffer[1024];
  int subsample = -1; /* no sub-sampling */
  //----------------------------------------------------------------
  zero_string(buffer, 1024);
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( grp_fld == NULL ) || ( *grp_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( key_fld == NULL ) || ( *key_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }

  if ( ( key1 == NULL ) || ( *key1 == '\0' ) ) { go_BYE(-1); }
  if ( ( key2 == NULL ) || ( *key2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt1 == NULL ) || ( *cnt1 == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt2 == NULL ) || ( *cnt2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(key1, key2) == 0 ) { go_BYE(-1); }
  if ( strcmp(cnt1, cnt2) == 0 ) { go_BYE(-1); }
  if ( strcmp(key1, cnt1) == 0 ) { go_BYE(-1); }
  if ( strcmp(key2, cnt2) == 0 ) { go_BYE(-1); }

  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( ( str_subsample == NULL ) || ( *str_subsample == '\0' ) ) {
    subsample = -1;
  }
  else {
    char *endptr;
    subsample = strtoll(str_subsample, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( subsample <= 1 ) { go_BYE(-1); }
  }
  //-------------------------------------------
  void *Tzp = NULL;
  Tps = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tps == NULL ) { go_BYE(-1); }
  Tpf = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tpf == NULL ) { go_BYE(-1); }
  Tzp = 0;
  //-------------------------------------------
  gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
  long long t_before_usec = (long long int)Tps->tv_usec;
  srand48(t_before_usec);
  //-------------------------------------------
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  if ( src_nR == 0 ) { goto BYE; }
  //--------------------------------------------------------
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0 ) { 
    status = del_tbl(NULL, dst_tbl_id); cBYE(status);
  }
  //--------------------------------------------------------
  // Get the grp_fld. Cannot have null values. Must be int
  status = is_fld(NULL, src_tbl_id, grp_fld, &grp_fld_id); cBYE(status);
  chk_range(grp_fld_id, 0, g_n_fld);
  grp_fld_meta = &(g_fld[grp_fld_id]);
  status = rs_mmap(grp_fld_meta->filename, &grp_fld_X, &grp_fld_nX, 0); 
  cBYE(status);
  nn_grp_fld_id = grp_fld_meta->nn_fld_id;
  if ( strcmp(grp_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( nn_grp_fld_id >= 0 ) {  go_BYE(-1); }
  //--------------------------------------------------------
  // Get the key_fld. Cannot have null values. Must be int
  status = is_fld(NULL, src_tbl_id, key_fld, &key_fld_id); cBYE(status);
  chk_range(key_fld_id, 0, g_n_fld);
  key_fld_meta = &(g_fld[key_fld_id]);
  status = rs_mmap(key_fld_meta->filename, &key_fld_X, &key_fld_nX, 0); 
  cBYE(status);
  nn_key_fld_id = key_fld_meta->nn_fld_id;
  if ( strcmp(key_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( nn_key_fld_id >= 0 ) {  go_BYE(-1); }
  //--------------------------------------------------------
  // Get the cnt. Cannot have null values. Must be int
  status = is_fld(NULL, src_tbl_id, cnt_fld, &cnt_fld_id); cBYE(status);
  chk_range(cnt_fld_id, 0, g_n_fld);
  cnt_fld_meta = &(g_fld[cnt_fld_id]);
  status = rs_mmap(cnt_fld_meta->filename, &cnt_fld_X, &cnt_fld_nX, 0);
  cBYE(status);
  nn_cnt_fld_id = cnt_fld_meta->nn_fld_id;
  if ( strcmp(cnt_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( nn_cnt_fld_id >= 0 ) {  go_BYE(-1); }
  //--------------------------------------------------------
  // Open files for writing
  status = open_temp_file(&key1fp, &key1fname, -1); cBYE(status);
  status = open_temp_file(&key2fp, &key2fname, -1); cBYE(status);
  status = open_temp_file(&cnt1fp, &cnt1fname, -1); cBYE(status);
  status = open_temp_file(&cnt2fp, &cnt2fname, -1); cBYE(status);
  //--------------------------------------------------------
  dst_nR = 0;
  int *grp_ptr = (int *)grp_fld_X;
  int *key_ptr = (int *)key_fld_X;
  int *cnt_ptr = (int *)cnt_fld_X;
  for ( long long i = 0; i < src_nR; ) { /* i is incremented below */
    long long j;
    int grp_val, x_grp_val; 
    int key_val_1, key_val_2;
    int cnt_val_1, cnt_val_2;
    grp_val = grp_ptr[i];
    for ( j = i + 1; j < src_nR; j++ ) { 
      x_grp_val = grp_ptr[j];
      if ( x_grp_val != grp_val ) { break; }
    }
    long long lb_incl = i;
    long long ub_excl = j;
    // Compute sums for all pairs
    for ( long long k1 = lb_incl; k1 < ub_excl; k1++ ) {
      long int litemp;
      key_val_1 = key_ptr[k1];
      cnt_val_1 = cnt_ptr[k1];
      /* Note that we do the upper diagonal of the pairs matrix */
      for ( long long k2 = k1+1; k2 < ub_excl; k2++ ) { 
	bool is_write;
        key_val_2 = key_ptr[k2];
	if ( key_val_1 == key_val_2 ) { continue; }
        cnt_val_2 = cnt_ptr[k2];
        if ( ( cnt_val_1 == 0 )  && ( cnt_val_2 == 0 ) ) {
  	  continue; /* TODO: Document this fact */
        }
	if ( subsample > 1 ) { 
	  litemp = mrand48();
	  if ( litemp < 0 ) { litemp *= -1; }
	  litemp = litemp % subsample;
	  if ( litemp == 0 ) {
	    is_write = true;
	  }
	  else {
	    is_write = false;
	  }
	}
	else {
	  is_write = true;
	}
	if ( is_write ) { 
	fwrite(&key_val_1, sizeof(int), 1, key1fp);
	fwrite(&key_val_2, sizeof(int), 1, key2fp);
	fwrite(&cnt_val_1, sizeof(int), 1, cnt1fp);
	fwrite(&cnt_val_2, sizeof(int), 1, cnt2fp);
	dst_nR++;
	}
      }
    }
    i = j;
  }
  fclose_if_non_null(key1fp);
  fclose_if_non_null(key2fp);
  fclose_if_non_null(cnt1fp);
  fclose_if_non_null(cnt2fp);
    
  int itemp; 
  sprintf(buffer, "%lld", dst_nR);
  status = add_tbl(dst_tbl, buffer, &dst_tbl_id); cBYE(status);

  sprintf(buffer, "fldtype=%s:filename=%s:n_sizeof=%d", 
      key_fld_meta->fldtype, key1fname, key_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, key1, buffer, &itemp); cBYE(status);

  sprintf(buffer, "fldtype=%s:filename=%s:n_sizeof=%d", 
      key_fld_meta->fldtype, key2fname, key_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, key2, buffer, &itemp); cBYE(status);

  sprintf(buffer, "fldtype=%s:filename=%s:n_sizeof=%d", 
      cnt_fld_meta->fldtype, cnt1fname, cnt_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, cnt1, buffer, &itemp); cBYE(status);

  sprintf(buffer, "fldtype=%s:filename=%s:n_sizeof=%d", 
      cnt_fld_meta->fldtype, cnt2fname, cnt_fld_meta->n_sizeof);
  status = add_fld(dst_tbl, cnt2, buffer, &itemp); cBYE(status);


  //--------------------------------------------------------
 BYE:
  rs_munmap(grp_fld_X, grp_fld_nX);
  rs_munmap(key_fld_X, key_fld_nX);
  rs_munmap(cnt_fld_X, cnt_fld_nX);

  fclose_if_non_null(key1fp);
  fclose_if_non_null(key2fp);
  fclose_if_non_null(cnt1fp);
  fclose_if_non_null(cnt2fp);

  free_if_non_null(key1fname); free_if_non_null(key2fname);
  free_if_non_null(cnt1fname); free_if_non_null(cnt2fname); 

  free_if_non_null(Tps);
  free_if_non_null(Tpf);

  return(status);
}
