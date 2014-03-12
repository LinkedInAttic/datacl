#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
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
#include "f1opf2.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "chunking.h"
#include "meta_globals.h"

static int g_num_rows_rng[MAX_NUM_THREADS];
static int g_num_rows_src[MAX_NUM_THREADS];
static int g_thread_id[MAX_NUM_THREADS];
static int g_nT; /* num_threads */
static long long g_src_nR;
static long long g_rng_nR;
static int *g_lbptr = NULL;
static int *g_ubptr = NULL;
static char *g_dst_fld_X = NULL;
static char *g_src_fld_X = NULL;
static long long *g_cumptr = NULL;
static long long *g_cntptr = NULL;

void *core_mv_range(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  // De-reference global variables
  int nT = g_nT;
  long long src_nR = g_src_nR;
  long long rng_nR = g_rng_nR;
  int *lbptr = g_lbptr;
  int *ubptr = g_ubptr;
  char *dst_fld_X = g_dst_fld_X;
  char *src_fld_X = g_src_fld_X;
  long long *cumptr = g_cumptr;
  long long *cntptr = g_cntptr;
  //------------------------------------------------------

  int *outptr = NULL;
  int *inptr = NULL;
  for ( int t = 0; t < rng_nR; t++ ) { 
    int inval, t_lb, t_ub;
    if ( ( t % nT) != tid ) { continue; }
    g_num_rows_rng[tid] += 1;
    long long my_count = 0;
    t_lb =  lbptr[t];
    t_ub =  ubptr[t];
    outptr = (int *)dst_fld_X;
    if ( t > 0 ) { 
      outptr += cumptr[t-1];
    }
    inptr = (int *)src_fld_X;
    for ( long long i = 0; i < src_nR; i++ ) { 
      inval = *inptr;
      if ( ( inval  >= t_lb ) && ( inval <= t_ub ) ) {
	if ( my_count >= cntptr[t] ) { go_BYE(-1); }
	*outptr = inval;
	outptr++;
	my_count++;
      }
      inptr++;
    }
    if ( my_count != cntptr[t] ) { go_BYE(-1); }
    g_num_rows_src[tid] += my_count;
  }
BYE:
  if ( status == 0 ) {
    return ((void *)0);
  }
  else {
    return ((void *)1); }
}
//---------------------------------------------------------------
// START FUNC DECL
int 
mv_range(
	   char *src_tbl,
	   char *src_fld,
	   char *dst_fld,
	   char *rng_tbl,
	   char *lb_fld, /* inclusive */
	   char *ub_fld, /* exclusive */
	   char *cnt_fld 
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  char *dst_fld_X = NULL; size_t dst_fld_nX = 0;
  char *lb_fld_X = NULL; size_t lb_fld_nX = 0;
  char *ub_fld_X = NULL; size_t ub_fld_nX = 0;
  char *cnt_fld_X = NULL; size_t cnt_fld_nX = 0;
  char *cum_fld_X = NULL; size_t cum_fld_nX = 0;
  FLD_TYPE *src_fld_meta = NULL;
  FLD_TYPE *lb_fld_meta = NULL;
  FLD_TYPE *ub_fld_meta = NULL;
  FLD_TYPE *cnt_fld_meta = NULL;
  FLD_TYPE *cum_fld_meta = NULL;
  long long src_nR, rng_nR;
  int src_tbl_id = INT_MIN, rng_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN;
  int lb_fld_id = INT_MIN, ub_fld_id = INT_MIN; 
  int cnt_fld_id = INT_MIN, cum_fld_id = INT_MIN;
  long long *cumptr = NULL, *cntptr = NULL;
  char str_meta_data[1024];
  char *opfile = NULL; 
  char cum_fld[MAX_LEN_FLD_NAME];
  // For multi-threading 
  int nT;
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( rng_tbl == NULL ) || ( *rng_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld == NULL ) || ( *ub_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, rng_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(src_fld, dst_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_fld, ub_fld) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(cum_fld, MAX_LEN_FLD_NAME);
  strcpy(cum_fld, "_cum_");
  int len = strlen(cum_fld);
  char *cptr1 = cum_fld; 
  char *cptr2 = cum_fld + len;
  for ( int i = len; i < MAX_LEN_FLD_NAME; i++ ) { 
    if ( *cptr1 == '\0' ) {  break; }
    *cptr2++ = *cptr1++;
  }
  *cptr2++ = '\0'; /* null terminate */

  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  src_fld_meta = &(g_fld[src_fld_id]);
  status = rs_mmap(src_fld_meta->filename, &src_fld_X, &src_fld_nX, 0); cBYE(status);
  // Cannot yet deal with following cases
  if ( strcmp(src_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( src_fld_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(rng_tbl, &rng_tbl_id); cBYE(status);
  chk_range(rng_tbl_id, 0, g_n_tbl);
  rng_nR = g_tbl[rng_tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, rng_tbl_id, lb_fld, &lb_fld_id); cBYE(status);
  chk_range(lb_fld_id, 0, g_n_fld);
  lb_fld_meta = &(g_fld[lb_fld_id]);
  status = rs_mmap(lb_fld_meta->filename, &lb_fld_X, &lb_fld_nX, 0); cBYE(status);
  if ( strcmp(lb_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  int * lbptr = (int *)lb_fld_X;
  //--------------------------------------------------------
  status = is_fld(NULL, rng_tbl_id, ub_fld, &ub_fld_id); cBYE(status);
  chk_range(ub_fld_id, 0, g_n_fld);
  ub_fld_meta = &(g_fld[ub_fld_id]);
  status = rs_mmap(ub_fld_meta->filename, &ub_fld_X, &ub_fld_nX, 0); cBYE(status);
  if ( strcmp(ub_fld_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  int * ubptr = (int *)ub_fld_X;
  //--------------------------------------------------------
  status = is_fld(NULL, rng_tbl_id, cnt_fld, &cnt_fld_id); cBYE(status);
  chk_range(cnt_fld_id, 0, g_n_fld);
  cnt_fld_meta = &(g_fld[cnt_fld_id]);
  status = rs_mmap(cnt_fld_meta->filename, &cnt_fld_X, &cnt_fld_nX, 0); cBYE(status);
  if ( strcmp(cnt_fld_meta->fldtype, "long long") != 0 ) { go_BYE(-1); }
  cntptr = (long long *)cnt_fld_X;
  // Create output
  status = mk_temp_file(opfile, src_nR * src_fld_meta->n_sizeof); cBYE(status);
  status = q_mmap(opfile, &dst_fld_X, &dst_fld_nX, 1); cBYE(status);
  //------------------------------------------------------
  status = f1opf2(rng_tbl, cnt_fld, "op=cum", cum_fld); cBYE(status);
  status = is_fld(NULL, rng_tbl_id, cum_fld, &cum_fld_id); cBYE(status);
  chk_range(cum_fld_id, 0, g_n_fld);
  cum_fld_meta = &(g_fld[cum_fld_id]);
  status = rs_mmap(cum_fld_meta->filename, &cum_fld_X, &cum_fld_nX, 0); cBYE(status);
  if ( strcmp(cum_fld_meta->fldtype, "long long") != 0 ) { go_BYE(-1); }
  cumptr = (long long *)cum_fld_X;

  //------------------------------------------------------
  // Set up global variables
  g_src_nR = src_nR;
  g_rng_nR = rng_nR;
  g_lbptr = lbptr;
  g_ubptr = ubptr;
  g_dst_fld_X = dst_fld_X;
  g_src_fld_X = src_fld_X;
  g_cumptr = cumptr;
  g_cntptr = cntptr;
  //------------------------------------------------------
  //--- Decide on how much parallelism to use
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
    g_num_rows_rng[i] = 0;
    g_num_rows_src[i] = 0;
  }
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_MV_RANGE 2
  if ( rng_nR <= MIN_ROWS_FOR_MV_RANGE ) {
    nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( nT > rng_nR ) { nT = rng_nR; }
  //--------------------------------------------
  g_nT = nT;
  // START Computation
  long long chk_rng_nR = 0;
  long long chk_src_nR = 0;
  if ( g_nT == 1 ) { 
    core_mv_range(&(g_thread_id[0]));
    chk_rng_nR = g_num_rows_rng[0];
    chk_src_nR = g_num_rows_src[0];
  }
  else {
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_mv_range,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_rng_nR += g_num_rows_rng[t];
      chk_src_nR += g_num_rows_src[t];
    }
  }
  if ( chk_rng_nR != rng_nR ) { go_BYE(-1); }
  if ( chk_src_nR != src_nR ) { go_BYE(-1); }
  //------------------------------------------------------
  // Add dst_fld to meta data 
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s", 
      src_fld_meta->fldtype, src_fld_meta->n_sizeof, opfile);
  status = add_fld(src_tbl, dst_fld, str_meta_data, &dst_fld_id);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  rs_munmap(src_fld_X, src_fld_nX);
  rs_munmap(dst_fld_X, dst_fld_nX);
  rs_munmap(lb_fld_X, lb_fld_nX);
  rs_munmap(ub_fld_X, ub_fld_nX);
  rs_munmap(cnt_fld_X, cnt_fld_nX);
  rs_munmap(cum_fld_X, cum_fld_nX);
  free_if_non_null(opfile);
  return(status);
}
