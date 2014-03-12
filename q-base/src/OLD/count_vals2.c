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
#include "f_to_s.h"
#include "chunking.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;

static int g_num_rows[MAX_NUM_THREADS];
static int g_thread_id[MAX_NUM_THREADS];
static int g_nT; /* num_threads */
static long long g_src_nR;
static long long **g_t_counts;
static int g_num_src_vals;
static int g_iminval;
static char *g_src_fld_X;

void *core_count_vals2(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  long long lb, ub, block_size;
  int *inptr = NULL; int ival, idx;
  // De-reference global variables
  int nT = g_nT;
  long long src_nR = g_src_nR;
  long long **t_counts = g_t_counts;
  char *src_fld_X = g_src_fld_X;
  int iminval = g_iminval;
  int num_src_vals = g_num_src_vals;
  //------------------------------------------------------
  
  block_size = src_nR / nT;
  for ( int t = 0; t < nT; t++ ) { 
    if ( ( t % nT) != tid ) { continue; }
    lb = t * block_size;
    ub = lb + block_size;
    if ( t == (nT-1) ) { ub = src_nR; }
    block_size = ub - lb;
    if ( block_size == 0 ) { continue; }
    inptr = (int *)src_fld_X;
    inptr += lb;
    g_num_rows[t] = block_size;
    long long *cntptr = t_counts[t];
    for ( long long i = lb; i < ub; i++ ) { 
      ival = *inptr;
      idx = ival - iminval;
      /* TODO: This is just for debugging. Remove later */
      if ( ( idx < 0 ) || ( idx >= num_src_vals ) ) { go_BYE(-1); }
      cntptr[idx]++;
      inptr++;
    }
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
count_vals2(
	   char *src_tbl,
	   char *src_fld,
	   char *dst_tbl,
	   char *val_fld,
	   char *cnt_fld
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *src_fld_X = NULL; size_t src_fld_nX = 0;
  FLD_TYPE *src_fld_meta = NULL;
  long long src_nR, dst_nR;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN;
  int src_fld_id = INT_MIN, val_fld_id = INT_MIN, cnt_fld_id = INT_MIN;
  char str_meta_data[1024]; char str_rslt[32];
  char *cntfile = NULL, *valfile = NULL; FILE *ofp = 0;
  int iminval, imaxval, num_src_vals; int *nz_ivals = NULL;
  long long l_num_src_vals;
  long long **t_counts = NULL;
  long long *counts = NULL;
  long long *nz_counts = NULL;
  // For multi-threading 
  int nT;
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( val_fld == NULL ) || ( *val_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt_fld == NULL ) || ( *cnt_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  if ( strcmp(val_fld, cnt_fld) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  zero_string(str_rslt, 32);

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
  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  if ( dst_tbl_id >= 0  ) { 
    status = del_tbl(dst_tbl, -1); cBYE(status);
  }
  //--------------------------------------------------------
  //--- Decide on how much parallelism to use
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
    g_num_rows[i] = 0;
  }
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_COUNT_VALS2 2
  if ( src_nR <= MIN_ROWS_FOR_COUNT_VALS2 ) {
    nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( nT > src_nR ) { nT = src_nR; }
  //--------------------------------------------------------
  // Create space for partial results from each thread
  char *endptr;
  status = f_to_s(src_tbl, src_fld, "min", str_rslt); cBYE(status);
  iminval = strtoll(str_rslt, &endptr, 10);
  status = f_to_s(src_tbl, src_fld, "max", str_rslt); cBYE(status);
  imaxval = strtoll(str_rslt, &endptr, 10);
  l_num_src_vals = (imaxval - iminval + 1 );
  if ( l_num_src_vals > 4096 ) { go_BYE(-1); }
  num_src_vals = l_num_src_vals;
  t_counts = malloc(nT * sizeof(long long *));
  return_if_malloc_failed(t_counts);
  for ( int i = 0; i < nT; i++ ) { 
    t_counts[i] = malloc(num_src_vals * sizeof(long long));
    return_if_malloc_failed(t_counts[i]);
  }
  for ( int i = 0; i < nT; i++ ) { 
    for ( int j = 0; j < num_src_vals; j++ ) { 
      t_counts[i][j] = 0;
    }
  }
  counts = malloc(num_src_vals * sizeof(long long));
  return_if_malloc_failed(counts);
  for ( int i = 0; i < num_src_vals; i++ ) { 
    counts[i] = 0;
  }
  //------------------------------------------------------
  // Set up global variables
  g_src_nR = src_nR;
  g_nT = nT;
  g_t_counts = t_counts;
  g_num_src_vals = num_src_vals;
  g_iminval = iminval;
  g_src_fld_X = src_fld_X;
  // START Computation
  long long chk_src_nR = 0;
  if ( g_nT == 1 ) { 
    core_count_vals2(&(g_thread_id[0]));
    chk_src_nR = g_num_rows[0];
  }
  else {
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_count_vals2,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_src_nR += g_num_rows[t];
    }
  }
  if ( chk_src_nR != src_nR ) { go_BYE(-1); }
  //------------------------------------------------------
  /* Accumulate values across different threads */
  chk_src_nR = 0;
  for ( int t = 0; t < nT; t++ ) { 
    for ( int j = 0; j < num_src_vals; j++ ) { 
      long long ltemp;
      ltemp = t_counts[t][j];
      counts[j] += ltemp;
      chk_src_nR += ltemp;
    }
  }
  if ( chk_src_nR != src_nR ) { go_BYE(-1); }
  //------------------------------------------------------
  /* Eliminate zero values */
  dst_nR = 0; 
  for ( int j = 0; j < num_src_vals; j++ ) { 
    if ( counts[j] > 0 ) { 
      dst_nR++;
    }
  }
  /* Create counts for non-zero values */
  nz_counts = malloc(dst_nR * sizeof(long long));
  return_if_malloc_failed(nz_counts);
  nz_ivals = malloc(dst_nR * sizeof(int));
  return_if_malloc_failed(nz_ivals);
  int j = 0;
  for ( int i = 0;i < num_src_vals; i++ ) { 
    if ( counts[i] > 0 ) {
      nz_ivals[j] = i + iminval;
      nz_counts[j] = counts[i];
      j++;
    }
  }
  // Write out counts and vals 
  status = open_temp_file(&ofp, &valfile, -1); cBYE(status);
  fwrite(nz_ivals, sizeof(int), dst_nR, ofp);
  fclose_if_non_null(ofp);

  status = open_temp_file(&ofp, &cntfile, -1); cBYE(status);
  fwrite(nz_counts, sizeof(long long), dst_nR, ofp);
  fclose_if_non_null(ofp);


  //------------------------------------------------------
  // Add dst_tbl
  status = del_tbl(dst_tbl, -1);
  sprintf(str_rslt, "%lld", dst_nR);
  status = add_tbl(dst_tbl, str_rslt, &dst_tbl_id); cBYE(status);
  // Add val fld 
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s", 
      src_fld_meta->fldtype, src_fld_meta->n_sizeof, valfile);
  status = add_fld(dst_tbl, val_fld, str_meta_data, &val_fld_id);
  cBYE(status);
  // Add cnt_fld to meta data 
  sprintf(str_meta_data, "fldtype=long long:n_sizeof=8:filename=%s", cntfile);
  status = add_fld(dst_tbl, cnt_fld, str_meta_data, &cnt_fld_id);
  cBYE(status);
  //-----------------------------------------------------------
 BYE:
  if ( t_counts != NULL ) { 
    for ( int t = 0; t < nT; t++ ) { 
      free_if_non_null(t_counts[t]);
    }
    free_if_non_null(t_counts);
  }
  free_if_non_null(counts);
  rs_munmap(src_fld_X, src_fld_nX);
  free_if_non_null(cntfile);
  free_if_non_null(valfile);
  free_if_non_null(nz_counts);
  free_if_non_null(nz_ivals);
  return(status);
}
