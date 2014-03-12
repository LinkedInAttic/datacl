#ifdef IPP
#include "ipps.h"
#endif
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
#include "mk_temp_file.h"
#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "vec_f1opf2.h"
#include "aux_meta.h"
#include "mk_bins.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1opf2.h"
#include "f1f2opf3.h"
#include "f1s1opf2.h"
#include "fop.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "drop_nn_fld.h"
#include "subsample.h"
#include "num_in_range.h"
#include "mv_range.h"
#include "set_meta.h"
#include "meta_globals.h"

extern int g_num_cores;
extern int g_alt_ddir_id;

#define MIN_COUNT_FOR_PARALLEL_SORT 1048576
//---------------------------------------------------------------
// START FUNC DECL
int 
parsort1(
	 char *t1,
	 char *f1,
	 char *srt_direction,
	 char *f2
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *lb_X = NULL; size_t lb_nX = 0;
  char *ub_X = NULL; size_t ub_nX = 0;
  char *cnt_X = NULL; size_t cnt_nX = 0;
  char *t3_cnt_X = NULL; size_t t3_cnt_nX = 0;
  char *t2_f2_X = NULL; size_t t2_f2_nX = 0;
  int ddir_id;
  TBL_REC_TYPE t1_rec, t2_rec, t3_rec;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE lb_rec, nn_lb_rec;
  FLD_REC_TYPE ub_rec, nn_ub_rec;
  FLD_REC_TYPE cnt_rec, nn_cnt_rec;
  FLD_REC_TYPE f2_rec;
  FLD_REC_TYPE t2_f2_rec;
  long long nR1, nR2, nR3;
  int t1_id = INT_MIN, t2_id = INT_MIN, t3_id = INT_MIN;
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int lb_id = INT_MIN, nn_lb_id = INT_MIN;
  int ub_id = INT_MIN, nn_ub_id = INT_MIN;
  int cnt_id = INT_MIN, nn_cnt_id = INT_MIN;
  int f2_id = INT_MIN, nn_f2_id = INT_MIN;
  int t2_f2_id = INT_MIN;
  char opfile[MAX_LEN_FILE_NAME+1];
  char **partfile = NULL;
  FILE *tfp = NULL; char *tempfile = NULL;
  char str_reduce_fac[32], str_num_bins[32]; 
  char t2[MAX_LEN_TBL_NAME], t3[MAX_LEN_TBL_NAME];
  int fldsz;
  long long chk_nR1 = 0;
  long long *t3_cnt_I4 = NULL;
  long long *bufsz  = NULL;
  int *part_ddir_id = NULL;
  char **partX = NULL; size_t *part_nX = NULL;
  long long *cum_cnt = NULL;
  int bak_alt_ddir_id = g_alt_ddir_id;

  if ( ( strcmp(srt_direction, "ascending") != 0 ) &&
       ( strcmp(srt_direction, "descending") != 0 ) ) {
    go_BYE(-1);
  }
  zero_string(opfile, (MAX_LEN_FILE_NAME+1));
  zero_string(str_reduce_fac, 32);
  zero_string(str_num_bins, 32);
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  /* t2 is a temporary table, created by sub-sampling t1 */
  zero_string(t2, MAX_LEN_TBL_NAME);
  status = qd_uq_str(t2, MAX_LEN_TBL_NAME);
  strcpy(t2, "t2"); // TODO DELETE THIS WHEN DEBUGGING IS DONE
  /* t3 is a temporary table created when we make bins out of t2 */
  zero_string(t3, MAX_LEN_TBL_NAME);
  status = qd_uq_str(t3, MAX_LEN_TBL_NAME);
  strcpy(t3, "t3"); // TODO DELETE THIS WHEN DEBUGGING IS DONE

  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = t1_rec.nR;
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, 0); cBYE(status);
  // Not implemented for following cases 
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != I4 ) { go_BYE(-1); }
  if ( nR1 <= MIN_COUNT_FOR_PARALLEL_SORT ) { 
    // Just do a sequential sort and return 
  }
  //---------------------------------------------
#define NR2 65536
  int reduction_factor = nR1 / NR2;
  status = subsample(t1, f1, "65536", t2, "f1"); cBYE(status);
  // Sort the values that we sampled
  status = fop(t2, "f1", "sortA"); cBYE(status);
  status = get_nR(t2, &nR2); cBYE(status);
  /* Make bins */
  int num_bins = 2 * g_num_cores;
  if ( num_bins > nR2 ) { go_BYE(-1); } // TODO P3: Handle this case
  sprintf(str_num_bins, "%d", num_bins);
  status = mk_bins(t2, "f1", t3, str_num_bins); cBYE(status);
  status = is_tbl(t3, &t3_id, &t3_rec);
  nR3 = t3_rec.nR;

  status = is_fld(NULL, t3_id, "lb", &lb_id, &lb_rec, &nn_lb_id, &nn_lb_rec);
  cBYE(status);
  status = get_data(lb_rec, &lb_X, &lb_nX, 1); cBYE(status);

  status = is_fld(NULL, t3_id, "ub", &ub_id, &ub_rec, &nn_ub_id, &nn_ub_rec);
  cBYE(status);
  status = get_data(ub_rec, &ub_X, &ub_nX, 1); cBYE(status);

  status = is_fld(NULL, t3_id, "cnt", &cnt_id, &cnt_rec,&nn_cnt_id,&nn_cnt_rec);
  cBYE(status);
  status = get_data(cnt_rec, &cnt_X, &cnt_nX, 1); cBYE(status);

  // Make space for f2 
  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
  status = mk_temp_file(opfile, (nR1 * fldsz), &ddir_id); cBYE(status);
  status = q_mmap(ddir_id, opfile, &f2_X, &f2_nX, true); cBYE(status);
  // Copy from f1 to f2 based on bins. TODO P3: Parallelize this
  int *inI4ptr = (int *)f1_X;
  int *lbI4ptr = (int *)lb_X;
  int *ubI4ptr = (int *)ub_X;
  long long *cntI8ptr = (long long *)cnt_X;
  // Set boundary conditions
  lbI4ptr[0] = INT_MIN;
  ubI4ptr[nR3-1] = INT_MAX;
  // Make bins boundaries line up
  for ( int i = 1; i < nR3; i++ ) { 
    int prev_ub = ubI4ptr[i-1];
    int curr_lb = lbI4ptr[i];
    if ( prev_ub == curr_lb ) { go_BYE(-1); }
    int midpoint = (int)(( (double)prev_ub + (double)curr_lb ) / 2.0);
    prev_ub = midpoint - 1; 
    curr_lb = midpoint;
    ubI4ptr[i-1] = prev_ub;
    lbI4ptr[i] = curr_lb;
  }
  /* Make space to write output for each bin */
  bufsz = malloc(sizeof(long long) * nR3);
  return_if_malloc_failed(bufsz);
  part_ddir_id = malloc(sizeof(int) * nR3);
  return_if_malloc_failed(part_ddir_id);
  partfile = malloc(sizeof(char *) * nR3);
  return_if_malloc_failed(partfile);
  partX = malloc(sizeof(char *) * nR3);
  return_if_malloc_failed(partX);
  part_nX = malloc(sizeof(size_t) * nR3);
  return_if_malloc_failed(part_nX);
  for ( int i = 0; i < nR3; i++ ) { 
    partfile[i] = malloc((MAX_LEN_FILE_NAME+1) * sizeof(char));
    return_if_malloc_failed(partfile[i]);
  }
  /* Now we do some shennanigans for writing partfiles to ramfs  */
  char *tmp_data_dir = getenv("Q_TMP_DATA_DIR");
  int tmp_ddir_id;
  if ( tmp_data_dir != NULL ) { 
    status = get_ddir_id(tmp_data_dir, g_ddirs, g_n_ddir, true, &tmp_ddir_id);
    cBYE(status);
    g_alt_ddir_id = tmp_ddir_id;
  }
  //-----------------------------------------------------------------
  for ( int i = 0; i < num_bins; i++ ) { 
    bufsz[i] = cntI8ptr[i] * (1.25 * reduction_factor);
    status = mk_temp_file(partfile[i], bufsz[i] * sizeof(int), &(part_ddir_id[i]));
    cBYE(status);
    status = q_mmap(part_ddir_id[i], partfile[i], &(partX[i]), 
	&(part_nX[i]), true); 
    cntI8ptr[i] = 0; // what we have up to now is an estimate
  }
  for ( long long i = 0; i < nR1; i++ ) {
    int inI4 = inI4ptr[i];  // get input value
    // Determine which bin it should go into
    int bid = -1; // bin number;
    for ( int j = 0; j < nR3; j++ ) { 
      if ( ( inI4 >= lbI4ptr[j] ) && ( inI4 <= ubI4ptr[j] ) ) {
	bid = j;
	break;
      }
    }
    //-------------------------------------------------------
    if ( bid < 0 ) { 
      go_BYE(-1);
    }
    int *outptr = (int *)(partX[bid]);
    outptr[cntI8ptr[bid]++] = inI4;
    if ( cntI8ptr[bid] == bufsz[bid] ) {
      size_t curr_file_size = part_nX[bid];
      size_t new_file_size = max( curr_file_size + 65536, curr_file_size*1.25);
      size_t temp = new_file_size / 4096;
      new_file_size = ( temp + 1 ) * 4096;
      status = q_re_mmap(part_ddir_id[bid], partfile[bid], &(partX[bid]), 
	  &(part_nX[bid]), new_file_size, true);
      cBYE(status);
      // update buffer size 
      bufsz[bid] = new_file_size / sizeof(int);
    }
  }
  // check bin sizes
  chk_nR1 = 0;
  for ( int i = 0; i < nR3; i++ ) { 
    chk_nR1 += cntI8ptr[i];

  }
  // check file sizes
  chk_nR1 = 0;
  for ( int i = 0; i < nR3; i++ ) { 
    chk_nR1 += part_nX[i];
  }
  if ( chk_nR1 < ( nR1 * sizeof(int) ) ) { go_BYE(-1); }
  // Each thread sorts its own bin
  cilkfor ( int i = 0; i < nR3; i++ ) {
    long long num_in_bin = cntI8ptr[i];
    char *X = NULL; size_t nX = 0;
    status = q_mmap(part_ddir_id[i], partfile[i], &X, &nX, true); 
    if ( status < 0  ) { status = -1; continue; }
#undef IPP
#ifdef IPP
    if ( f1_rec.fldtype != I4 ) { status = -1; continue; }
    if ( num_in_bin >= INT_MAX ) { status = -1; continue; }
    if ( strcmp(srt_direction, "ascending") == 0 ) {
      ippsSortAscend_32s_I((int *)X, (int)num_in_bin);
    }
    else if ( strcmp(srt_direction, "ascending") == 0 ) {
      ippsSortDescend_32s_I((int *)X, (int)num_in_bin);
    }
    else { status = -1; continue; } 
#else
    if ( strcmp(srt_direction, "ascending") == 0 ) {
      qsort_asc_I4((int *)X, num_in_bin, fldsz, NULL);
    }
    else if ( strcmp(srt_direction, "descending") == 0 ) {
      qsort_dsc_I4((int *)X, num_in_bin, fldsz, NULL);
    }
    else { status = -1; continue; } 
#endif
    munmap(X, nX); X = NULL; nX = 0;
  }
  // make cumulative count from count 
  cum_cnt = malloc(nR3 * sizeof(long long));
  return_if_malloc_failed(cum_cnt);
  cum_cnt[0] = cntI8ptr[0];
  for ( int i = 1; i < nR3; i++ ) { 
    cum_cnt[i] = cntI8ptr[i] + cum_cnt[i-1];
  }
  if ( cum_cnt[nR3-1] != nR1 ) { go_BYE(-1); }
  cBYE(status);
  // Combine part files into output file 
  // below is cilk loop
  cilkfor ( int i = 0; i < nR3; i++ ) {
    long long offset;
    if ( i == 0 ) { offset = 0; } else { offset = cum_cnt[i-1]; }
    offset *= sizeof(int);
#ifdef IPP
// IppStatus ippsCopy_32s(const Ipp32s* pSrc, Ipp32s* pDst, int len);
    int *pSrc = (int *)(partX[i]);
    int *pDst = (int *)(f2_X + offset);
    int len   = cntI8ptr[i];
    ippsCopy_32s(pSrc, pDst, len);
#else
    memcpy(f2_X + offset, partX[i], cntI8ptr[i] * sizeof(int));
#endif
    munmap(partX[i], part_nX[i]);
    unlink(partfile[i]);
    free_if_non_null(partfile[i]);
  }

  status = add_fld(t1_id, f2, ddir_id, opfile, f1_rec.fldtype, -1, &f2_id, &f2_rec);
  cBYE(status);
  // Indicate the dst_fld is sorted ascending or descending
  status = set_meta(t1, f2, "sort_type", srt_direction); cBYE(status);
  /* Clean up temporary stuff */
  status = del_tbl(t2, -1); cBYE(status);
  status = del_tbl(t3, -1); cBYE(status);
BYE:
  g_alt_ddir_id = bak_alt_ddir_id;
  free_if_non_null(t3_cnt_I4);
  free_if_non_null(cum_cnt);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(lb_X, lb_nX);
  rs_munmap(ub_X, ub_nX);
  free_if_non_null(partfile);
  free_if_non_null(part_ddir_id);
  free_if_non_null(partX);
  free_if_non_null(part_nX);
  return(status);
}
