#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "qsort_asc_int.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "f1opf2_cum.h"
#include "f1opf2_shift.h"
#include "f1opf2_hash.h"
#include "vec_f1opf2.h"
#include "aux_fld_meta.h"
#include "str_xform.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1opf2.h"
#include "f1f2opf3.h"
#include "f1s1opf2.h"
#include "s_to_f.h"
#include "f_to_s.h"
#include "fop.h"
#include "pr_fld.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "drop_nn_fld.h"
#include "subsample.h"
#include "num_in_range.h"
#include "mv_range.h"
#include "set_fld_info.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;
static int g_thread_id[MAX_NUM_THREADS];
static int g_nT; /* num_threads */
static int **g_offsets;
static long long *g_count;


void *core_parsort1(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  /* "De-reference global variables */
  int nT = g_nT;
  long long *count = g_count;
  int **offsets = g_offsets;
  //------------------------------------------------
  if ( ( tid < 0 ) || ( tid > nT ) ) { go_BYE(-1); }
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    qsort_asc_int(offsets[t], count[t], sizeof(int), NULL);
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
parsort1(
       char *tbl,
       char *f1,
       char *f2,
       char *up_or_down /* not used right now */
       )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;
  char *t2f2_X = NULL; size_t t2f2_nX = 0;
  FLD_TYPE *f1_meta = NULL; 
  FLD_TYPE *f2_meta = NULL; 
  FLD_TYPE *t2f2_meta = NULL; 
  FLD_TYPE *cnt_meta = NULL; 
  long long nR, nR2;
  int tbl_id = INT_MIN, f1_id = INT_MIN, f2_id = INT_MIN, cnt_id = INT_MIN;
  int t2f2_id = INT_MIN;
  char str_meta_data[1024];
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *tfp = NULL; char *tempfile = NULL;
  char str_rslt[32]; zero_string(str_rslt, 32);
  char t2[MAX_LEN_TBL_NAME]; 
  int itemp;
  int *xxx = NULL, *f1lb = NULL, *f1ub = NULL; 
  long long *count = NULL, *chk_count = NULL;
  int **offsets = NULL, **bak_offsets = NULL;
  int *inptr = NULL;
  // For multi-threading 
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  // START: For timing
  struct timeval Tps;
  struct timezone Tpf;
  void *Tzp = NULL;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;
  long long t_after_sec, t_after_usec, t_after;
  long long t_delta_usec;
  // STOP : For timing
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  /* t2 isa temporary table */
  zero_string(t2, MAX_LEN_TBL_NAME);
  status = qd_uq_str(t2, MAX_LEN_TBL_NAME);
  strcpy(t2, "t2"); // TODO DELETE THIS 
  g_offsets = NULL;
  g_count = NULL;
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbl[tbl_id].nR;
  status = is_fld(NULL, tbl_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); 
  cBYE(status);
  // Not implemented for following cases 
  if ( g_fld[f1_id].nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( strcmp(f1_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( nR <= 1048576 ) { go_BYE(-1); }
  //---------------------------------------------
  status = gettimeofday(&Tps, &Tpf); cBYE(status);
  t_before_sec  = (long long)Tps.tv_sec;
  t_before_usec = (long long)Tps.tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;

  int reduction_factor = (int)(sqrt((double)nR));
  sprintf(str_rslt, "%d", reduction_factor);
  status = subsample(tbl, f1, str_rslt, t2, "f2"); cBYE(status);

  status = gettimeofday(&Tps, &Tpf); cBYE(status);
  t_after_sec  = (long long)Tps.tv_sec;
  t_after_usec = (long long)Tps.tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  fprintf(stderr, "TIME0 = %lld \n", t_after - t_before); 
  t_before = t_after;


  // Must have sufficient diversity of values
  status = f1opf2(t2, "f2", "op=shift:val=-1", "nextf2"); cBYE(status);
  status = drop_nn_fld(t2, "nextf2"); cBYE(status);
  status = f1f2opf3(t2, "f2", "nextf2", "==", "x"); cBYE(status);
  status = f_to_s(t2, "x", "sum", str_rslt);
  char *endptr;
  long long lltemp = strtoll(str_rslt, &endptr, 10);
  if ( lltemp != 0 ) { go_BYE(-1); }
  //-------------------------------------------------
  // Get range of values of f1 
  status = f_to_s(tbl, f1, "max", str_rslt);
  int f1max = strtoll(str_rslt, &endptr, 10);
  status = f_to_s(tbl, f1, "min", str_rslt);
  int f1min = strtoll(str_rslt, &endptr, 10);
  //-------------------------------------------------
  // Now we sort the values that we sampled
  status = fop(t2, "f2", "sortA"); cBYE(status);
  // status = pr_fld(t2, "f2", "", stdout);
  status = get_nR(t2, &nR2);
  // Now each thread selects a range to work on
  int nT;
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
  }
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_PARSORT1 1048576
  if ( nR <= MIN_ROWS_FOR_PARSORT1 ) {
    nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( nT > nR ) { nT = nR; }
  //--------------------------------------------

  double block_size = (double)nR2 / (double)nT;
  status = is_fld(t2, -1, "f2", &t2f2_id); cBYE(status);
  chk_range(t2f2_id, 0, g_n_fld);
  t2f2_meta = &(g_fld[t2f2_id]);
  status = rs_mmap(t2f2_meta->filename, &t2f2_X, &t2f2_nX, 0); 
  cBYE(status);
  int *iptr = (int *)t2f2_X;
  xxx = malloc(nT * sizeof(int)); return_if_malloc_failed(xxx);
  f1lb = malloc(nT * sizeof(int)); return_if_malloc_failed(f1lb);
  f1ub = malloc(nT * sizeof(int)); return_if_malloc_failed(f1ub);
  /* FOR OLD_WAY 
  count = malloc(nT * sizeof(long long)); return_if_malloc_failed(count);
  */
  chk_count = malloc(nT * sizeof(long long));
  return_if_malloc_failed(chk_count);
  g_count = malloc(nT * sizeof(long long)); return_if_malloc_failed(g_count);

  for ( int i = 0; i < nT; i++ ) { 
    // FOR OLD_WAY count[i]= 0;
    chk_count[i]= 0;
    int j = i+1;
    long long idx = j * block_size;
    if ( idx >= nR2 ) { idx = nR2 -1 ; }
    int y = iptr[idx];
    xxx[i] = y;
    // fprintf(stdout,"idx = %lld: j = %d: y = %d \n", idx, j, y);
  }
  for ( int i = 0; i < nT; i++ ) { 
    if ( ( i == 0 ) && ( i == (nT - 1 ) ) ) {
      f1lb[i] = f1min;
      f1ub[i] = f1max;
    }
    else if ( i == 0 ) { 
      f1lb[i] = f1min;
      f1ub[i] = xxx[i];
    }
    else if ( i == (nT -1 ) ) {
      f1lb[i] = xxx[i-1] + 1;
      f1ub[i] = f1max;
    }
    else {
      f1lb[i] = xxx[i-1] + 1;
      f1ub[i] = xxx[i];
    }
  }
  // STOP: Each thread has now a range to work on
  // Create a temporary table t3 to store ranges
  char t3[MAX_LEN_TBL_NAME]; int t3_id;
  zero_string(t3, MAX_LEN_TBL_NAME);
  status = qd_uq_str(t3, MAX_LEN_TBL_NAME);
  strcpy(t3, "t3"); // TODO DELETE THIS 
  sprintf(str_rslt, "%d", nT);
  status = add_tbl(t3, str_rslt, &t3_id);

  // Add lower bound to t3
  status = open_temp_file(&tfp, &tempfile, -1); cBYE(status);
  fclose_if_non_null(tfp);
  tfp = fopen(tempfile, "wb"); return_if_fopen_failed(tfp, tempfile, "wb");
  fwrite(f1lb, sizeof(int),  nT, tfp); 
  fclose_if_non_null(tfp);
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s",
      f1_meta->fldtype, f1_meta->n_sizeof, tempfile);
  status = add_fld(t3, "lb", str_meta_data, &itemp); cBYE(status);
  free_if_non_null(tempfile);

  // Add upper bound to t3
  status = open_temp_file(&tfp, &tempfile, -1); cBYE(status);
  fclose_if_non_null(tfp);
  tfp = fopen(tempfile, "wb"); return_if_fopen_failed(tfp, tempfile, "wb");
  fwrite(f1ub, sizeof(int),  nT, tfp); 
  fclose_if_non_null(tfp);
  sprintf(str_meta_data, "fldtype=%s:n_sizeof=%d:filename=%s",
      f1_meta->fldtype, f1_meta->n_sizeof, tempfile);
  status = add_fld(t3, "ub", str_meta_data, &itemp); cBYE(status);
  free_if_non_null(tempfile);

#undef OLD_WAY
#ifdef OLD_WAY
  // Now we count how much there is in each range 
  inptr = (int *)f1_X;
  for ( long long i = 0; i < nR; i++ ) { 
    int ival = *inptr++;
    int range_idx = INT_MIN;
    // TODO: Improve sequential search
    for ( int j = 0; j < nT; j++ ) { 
      if ( ival >= f1lb[j] && ( ival <= f1ub[j] ) ) {
	range_idx = j;
	break;
      }
    }
    count[range_idx]++;
  }
  /*
  for ( int i = 0; i < nT; i++ ) { 
    fprintf(stdout,"%d: (%d, %d) = %lld \n", i, f1lb[i], f1ub[i], count[i]);
  }
  */
#else
  status = num_in_range(tbl, f1, t3, "lb", "ub", "cnt"); cBYE(status);
  // Get a pointer to the count field 
  status = is_tbl(t3, &t3_id);
  chk_range(t3_id, 0, g_n_tbl);
  status = is_fld(NULL, t3_id, "cnt", &cnt_id);
  chk_range(cnt_id, 0, g_n_fld);
  cnt_meta = &(g_fld[cnt_id]); 
  status = rs_mmap(cnt_meta->filename, &cnt_X, &cnt_nX, 0); cBYE(status);
  count = (long long *)cnt_X;
#endif
  status = gettimeofday(&Tps, &Tpf); cBYE(status);
  t_after_sec  = (long long)Tps.tv_sec;
  t_after_usec = (long long)Tps.tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  fprintf(stderr, "TIME1 = %lld \n", t_after - t_before); 
  t_before = t_after;


  bak_offsets = malloc(nT * sizeof(int *)); return_if_malloc_failed(bak_offsets);
  g_offsets = malloc(nT * sizeof(int *)); return_if_malloc_failed(g_offsets);
#ifdef OLD_WAY
  // Make space for output 
  long long filesz = nR * f1_meta->n_sizeof;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &op_X, &op_nX, 1); cBYE(status);
  offsets = malloc(nT * sizeof(int *)); return_if_malloc_failed(offsets);
  long long cum_count = 0;
  for ( int i = 0; i < nT; i++ ) {
    bak_offsets[i] = offsets[i] = (int *)op_X;
    if ( i > 0 ) {
      cum_count += count[i-1];
      offsets[i] += cum_count;
      bak_offsets[i] = offsets[i];
    }
  }

  inptr = (int *)f1_X;
  // Now we place each item into its thread bucket
  for ( long long i = 0; i < nR; i++ ) { 
    int ival = *inptr++;
    int range_idx = INT_MIN;
    // TODO: Improve sequential search
    for ( int j = 0; j < nT; j++ ) { 
      if ( ival >= f1lb[j] && ( ival <= f1ub[j] ) ) {
	range_idx = j;
	break;
      }
    }
    int *xptr = offsets[range_idx];
    *xptr = ival;
    offsets[range_idx]++;
    chk_count[range_idx]++;
    if ( chk_count[range_idx] > count[range_idx] ) {
      go_BYE(-1);
    }
  }
  cum_count = 0;
  for ( int i = 0; i < nT-1; i++ ) { 
    if ( offsets[i] != bak_offsets[i+1] ) { 
      go_BYE(-1);
    }
  }
#else
  status = mv_range(tbl, f1, f2, t3, "lb", "ub", "cnt"); 
  cBYE(status);
  status = is_fld(NULL, tbl_id, f2, &f2_id);
  chk_range(f2_id, 0, g_n_fld);
  f2_meta = &(g_fld[f2_id]); 
  status = rs_mmap(f2_meta->filename, &op_X, &op_nX, 1); cBYE(status);
#endif

  long long cum_count = 0;
  for ( int i = 0; i < nT; i++ ) {
    bak_offsets[i] = (int *)op_X;
    if ( i > 0 ) {
      cum_count += count[i-1];
      bak_offsets[i] += cum_count;
    }
  }

  status = gettimeofday(&Tps, &Tpf); cBYE(status);
  t_after_sec  = (long long)Tps.tv_sec;
  t_after_usec = (long long)Tps.tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  fprintf(stderr, "TIME2 = %lld \n", t_after - t_before); 
  t_before = t_after;

  // Set up global variables
  g_nT = nT;
  for ( int i = 0; i < nT; i++ ) { 
    g_offsets[i] = bak_offsets[i];
    g_count[i] = count[i];
  }
  if ( g_nT == 1 ) { 
    core_parsort1(&(g_thread_id[0]));
  }
  else {
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_parsort1,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
    }
  }
  /* SEQUENTIAL CODE 
  for ( int i = 0; i < nT; i++ ) { 
    qsort_asc_int(bak_offsets[i], count[i], sizeof(int), NULL);
  }
  */
  status = gettimeofday(&Tps, &Tpf); cBYE(status);
  t_after_sec  = (long long)Tps.tv_sec;
  t_after_usec = (long long)Tps.tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  fprintf(stderr, "TIME3 = %lld \n", t_after - t_before); 

  // Indicate the dst_fld is sorted ascending
  status = set_fld_info(tbl, f2, "sort=1");


  rs_munmap(op_X, op_nX);
  status = del_tbl(t2, -1); cBYE(status);
  status = del_tbl(t3, -1); cBYE(status);
BYE:
  rs_munmap(op_X, op_nX);
  rs_munmap(cnt_X, cnt_nX);
  free_if_non_null(xxx);
  free_if_non_null(f1lb);
  free_if_non_null(f1ub);
  // Do not delete unless using OLD_WAY free_if_non_null(count);
  free_if_non_null(g_count);
  free_if_non_null(g_offsets);
  free_if_non_null(offsets);
  free_if_non_null(bak_offsets);
  free_if_non_null(chk_count);

  fclose_if_non_null(ofp);
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  rs_munmap(op_X, op_nX);
  free_if_non_null(opfile);
  return(status);
}
