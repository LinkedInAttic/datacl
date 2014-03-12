#include "qtypes.h"
#include "assign_I8.h"
#include "find_smallest_geq___XTYPE__.h"
#include "qsort_asc___XTYPE__.h"
#include "qsort_dsc___XTYPE__.h"

#define OLD
#ifdef OLD
static int
get_bin___XTYPE__(
    __ITYPE__ inval, 
    __ITYPE__ *lb, 
    __ITYPE__ *ub, 
    int num_bins, 
    int *ptr_b
    )
{
  int status = 0;
  /* This is sequential scan. Improve this */
  if ( inval <= lb[0]          ) { *ptr_b = 0;           return status; }
  if ( inval >= ub[num_bins-1] ) { 
    *ptr_b = num_bins -1; 
    return status; 
  }
  for ( int b = 0; b < num_bins; b++ ) {
    if ( inval < ub[b] ) {
      *ptr_b = b; return status;
    }
  }
// BYE:
  return(status);
}
#endif

// START FUNC DECL
int
par_sort_with_bins___XTYPE__(
    __ITYPE__  *in,  /* [nR] */
    long long nR,
    __ITYPE__  *binlb,  /* [num_bins] */
    __ITYPE__  *binub,  /* [num_bins] */
    int num_bins,
    char *srt_ordr,
    int nT,
    unsigned short *bin_idx, /* [nR] */
    __ITYPE__  *out /* [nR] */
    )
// STOP FUNC DECL
{
  int status = 0;
  long long *bin_offset = NULL;
  long long *bin_cnt    = NULL;
  long long **part_bin_cnt = NULL;

  // Following restriction because we have unsigned short for bin_idx
  // which means that we can handle up to 65536 bins
  if ( num_bins > 65536 ) { go_BYE(-1); }
  if ( nT < 1 ) { go_BYE(-1); }
  bin_offset = malloc(num_bins * sizeof(long long));
  return_if_malloc_failed(bin_offset);
  bin_cnt = malloc(num_bins * sizeof(long long));
  return_if_malloc_failed(bin_cnt);
  assign_const_I8(bin_cnt, num_bins, 0); 

  part_bin_cnt = malloc(nT * sizeof(long long *));
  return_if_malloc_failed(part_bin_cnt);
  for ( int tid = 0; tid < nT; tid++ ) { 
    part_bin_cnt[tid] = malloc(num_bins * sizeof(long long));
    return_if_malloc_failed(part_bin_cnt[tid]);
    assign_const_I8(part_bin_cnt[tid], num_bins, 0); 
  }

  // find number in each bin 
  if ( nR < nT ) { go_BYE(-1); }
  long long block_size = nR / nT;
  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    for ( long long i = lb; i < ub; i++ ) {
      __ITYPE__ inval = in[i];
      // long long b;
      // status = find_smallest_geq___XTYPE__(lb, num_bins, inval, &b); 
      int b;
      status = get_bin___XTYPE__(inval, binlb, binub, num_bins, &b);
      bin_idx[i] = b;
      bin_cnt[b]++;
    }
  }
  // Combine partial bin counts into bin count
  for ( int b = 0; b < num_bins; b++ ) {
    bin_cnt[b] = 0;
    for ( int tid = 0; tid < nT; tid++ ) {
      bin_cnt[b] += part_bin_cnt[tid][b];
    }
  }

  bin_offset[0] = 0;
  for ( int b = 1; b < num_bins; b++ ) {
    bin_offset[b] = bin_offset[b-1] + bin_cnt[b-1];
  }
  // put element in proper bin
  for ( long long i = 0; i < nR; i++ ) {
    __ITYPE__ inval = in[i];
    unsigned short b = bin_idx[i];
    long long idx = bin_offset[b];
    out[idx] = inval;
    bin_offset[b] = idx + 1;
  }
  // now starts the sort
  // fprintf(stderr, "Placed elements in correct bin\n");
#pragma omp parallel for
  for ( int b = 0; b < num_bins; b++ ) {
    char *X = (char *) out;
    X += (sizeof(int) * (bin_offset[b] - bin_cnt[b]) );
    qsort_asc___XTYPE__(X, bin_cnt[b], sizeof(__XTYPE__), NULL); 
  }
  // fprintf(stderr, "Completed sort\n");
BYE:
  free_if_non_null(bin_cnt);
  free_if_non_null(bin_offset);
  return(status);
}

#undef UNIT_TEST
#ifdef UNIT_TEST
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  __ITYPE__ *X = NULL, *Y = NULL;
  __ITYPE__ *binlb = NULL, *binub = NULL;
  unsigned short *bin_idx = NULL;
  long long N = 1048576;
  int num_bins = 1024;
  binlb = malloc(num_bins * sizeof(int));
  binub = malloc(num_bins * sizeof(int));
  bin_idx = malloc(N * sizeof(unsigned short));
  X       = malloc(N * sizeof(int));
  Y       = malloc(N * sizeof(int));
  for ( int i = 0; i < N; i++ ) { 
    X[i] = N-i;
  }
  binlb[0] = 0;
  for ( int i = 1; i < num_bins;i++ ) { 
    binlb[i] = binlb[i-1] + 1024;
  }
  for ( int i = 0; i < num_bins;i++ ) { 
    binub[i] = binlb[i] + 1024;
  }
  char *srt_ordr = "asc";
  status = par_sort_with_bins_I4(X, N, binlb, binub, num_bins, 
      srt_ordr, bin_idx, Y);
  cBYE(status);

BYE:
  free_if_non_null(X); 
  free_if_non_null(Y); 
  free_if_non_null(lb); 
  free_if_non_null(ub); 
  free_if_non_null(bin_idx); 
  return(status);
}
#endif
