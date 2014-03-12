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
#include "add_tbl.h"
#include "del_tbl.h"
#include "num_in_range.h"
#include "chunking.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;

static int g_num_rows[MAX_NUM_THREADS];
static int g_thread_id[MAX_NUM_THREADS];
static int g_nT; /* num_threads */
static int  *g_inptr;
static int  *g_lbptr;
static int *g_ubptr;
static long long **g_cntptrs;
static long long g_nR1;
static long long g_nR2;


void *core_num_in_range(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  /* "De-reference global variables */
  int nT = g_nT;
  int *inptr = g_inptr;
  int *lbptr = g_lbptr;
  int *ubptr = g_ubptr;
  long long nR1 = g_nR1;
  long long nR2 = g_nR2;
  long long block_size, lb, ub;
  long long **cntptrs = g_cntptrs;


  block_size = nR1 / nT;
  //------------------------------------------------
  if ( ( tid < 0 ) || ( tid > nT ) ) { go_BYE(-1); }
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    lb = t * block_size;
    ub = lb + block_size;
    if ( t == ( nT - 1 ) ) { ub = nR1; }
    block_size = ub - lb;
    g_num_rows[t] = block_size;
    inptr += lb;
    status = num_in_range_int(inptr, block_size, lbptr, ubptr, nR2,
	cntptrs[t]);
    cBYE(status);
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
num_in_range(
	       char *t1,
	       char *f1,
	       char *t2,
	       char *lb,
	       char *ub,
	       char *cnt
	       )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, lb_id = INT_MIN, ub_id = INT_MIN, cnt_id = INT_MIN;
  FLD_TYPE *f1_meta = NULL, *lb_meta = NULL, *ub_meta = NULL;
  long long nR1 = INT_MIN, nR2 = INT_MIN, chk_nR1 = INT_MIN;
  long long **cntptrs = NULL;
  // For multi-threading 
  int nT;
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;

  char str_meta_data[1024];
  char *opfile = NULL; FILE *ofp = NULL;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( lb == NULL ) || ( *lb == '\0' ) ) { go_BYE(-1); }
  if ( ( ub == NULL ) || ( *ub == '\0' ) ) { go_BYE(-1); }
  if ( ( cnt  == NULL ) || ( *cnt == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, t1_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); cBYE(status);
  // Have not implemented case where f1 has null field 
  if ( f1_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  // Have implemented only for int 
  if ( strcmp(f1_meta->fldtype, "int") != 0 ) { cBYE(-1); } 
  //--------------------------------------------------------
  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbl[t2_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, t2_id, lb, &lb_id); cBYE(status);
  chk_range(lb_id, 0, g_n_fld);
  lb_meta = &(g_fld[lb_id]);
  status = rs_mmap(lb_meta->filename, &lb_X, &lb_nX, 0); cBYE(status);
  // Have not implemented case where lb has null field 
  if ( lb_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  // Have implemented only for int 
  if ( strcmp(lb_meta->fldtype, "int") != 0 ) { cBYE(-1); } 
  //--------------------------------------------------------
  status = is_fld(NULL, t2_id, ub, &ub_id); cBYE(status);
  chk_range(ub_id, 0, g_n_fld);
  ub_meta = &(g_fld[ub_id]);
  status = rs_mmap(ub_meta->filename, &ub_X, &ub_nX, 0); cBYE(status);
  // Have not implemented case where ub has null field 
  if ( ub_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  // Have implemented only for int 
  if ( strcmp(ub_meta->fldtype, "int") != 0 ) { cBYE(-1); } 
  //--------------------------------------------------------
  // Set up access to input
  int *inptr = (int *)f1_X;
  int *lbptr = (int *)lb_X;
  int *ubptr = (int *)ub_X;
  //--------------------------------------------------------
  //--- Decide on how much parallelism to use
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
    g_num_rows[i] = 0;
  }
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_SUBSAMPLE 10000 // 1048576
  if ( nR1 <= MIN_ROWS_FOR_SUBSAMPLE ) {
    nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( nT > nR1 ) { nT = nR1; }
  //--------------------------------------------
  /* Make space for output */
  long long filesz = nR2 * sizeof(long long);
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &cnt_X, &cnt_nX, 1);
  long long *cntptr = (long long *)cnt_X;
  /* Make a holding tank for partial results */
  cntptrs = malloc(nT * sizeof(long long *));
  return_if_malloc_failed(cntptrs);
  for ( int i = 0; i < nT; i++ ) {
    cntptrs[i] = malloc(nR2 * sizeof(long long));
    return_if_malloc_failed(cntptrs[i]);
    for ( long long j = 0; j <nR2; j++ ) { 
      cntptrs[i][j] = 0;
    }
  }

  // Add count field to meta data 
  sprintf(str_meta_data, "fldtype=long long:n_sizeof=8:filename=%s", opfile);
  status = add_fld(t2, cnt, str_meta_data, &cnt_id); cBYE(status);
  chk_range(cnt_id, 0, g_n_fld);
  //-----------------------------------------------------------
  // Now we count how much there is in each range 
  // Set up global variables
  g_nT = nT;
  g_inptr = inptr;
  g_lbptr = lbptr;
  g_ubptr = ubptr;
  g_cntptrs = cntptrs;
  g_nR1 = nR1;
  g_nR2 = nR2;
  if ( g_nT == 1 ) { 
    core_num_in_range(&(g_thread_id[0]));
    chk_nR1 = g_num_rows[0];
  }
  else {
    chk_nR1 = 0;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_num_in_range,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_nR1 += g_num_rows[t];
    }
  }
  if ( chk_nR1 != nR1 ) { go_BYE(-1); }
  // Accumulate partial results
  for ( long long i = 0; i < nR2; i++ ) { 
    cntptr[i] = 0;
    for ( int j= 0;  j < nT; j++ ) { 
      cntptr[i] += cntptrs[j][i];
    }
  }

 BYE:
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  rs_munmap(cnt_X, cnt_nX);
  free_if_non_null(opfile);
  return(status);
}

int
num_in_range_int(
    int *inptr,
    long long nR1,
    int *lbptr,
    int *ubptr,
    long long nR2,
    long long *cntptr
    )
{
  int status = 0;
  for ( long long i = 0; i < nR1; i++ ) { 
    int ival = *inptr++;
    int range_idx = INT_MIN;
    // TODO: Improve sequential search
    for ( int j = 0; j < nR2; j++ ) { 
      if ( ival >= lbptr[j] && ( ival <= ubptr[j] ) ) {
	range_idx = j;
	break;
      }
    }
    cntptr[range_idx]++;
  }
  return(status);
}
