#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#ifdef MAC_OSX
#include "macstuff.h"
#endif
#include "assign_I8.h"
#ifdef IPP
#include "ipps.h"
#include "ippcore.h"
#endif

#include "qsort_asc_I4.h"
#include "qsort_dsc_I4.h"

#ifndef IPP 
#define ippGetCpuClocks get_time_usec
#endif

/* Parallelizes sort on the assumption that the elements are distributed
 * uniformly at random over the buckets created by masking out all but
 * the last 4 bits 
 * */

/* Important: Following 2 defines are inter-connected. Do not change one
 * without changing the other appropriately */

#define NUM_THREADS 64
#define MASK_FOR_NUM_THREADS 0x0000003F

// START FUNC DECL
int
srt_uniform_I4(
	       int *X, /* input */
	       long long nR,
	       int *Y, /* output */
	       int shift,
	       char *mode /* "asc" or "dsc" */
	       )
// STOP FUNC DECL
{
  int status = 0;
  long long **t_cnt = NULL, *cnt = NULL, *bin_offset = NULL; int nT = 0; 
  long long t[16];

  t[0] = ippGetCpuClocks();

  if ( shift < 0 ) { go_BYE(-1); }
  if ( X == NULL ) { go_BYE(-1); }
  if ( mode == NULL ) { go_BYE(-1); }
  if ( nR < 16*1048576 ) { 
    fprintf(stderr, "Too few elements to sort \n"); go_BYE(-1); 
  }
  if ( ( strcmp(mode, "asc") != 0 ) && ( strcmp(mode, "dsc") != 0 ) ) {
    go_BYE(-1); 
  }
  nT = NUM_THREADS;
  long long block_size = nR / nT;

  cnt = malloc(nT * sizeof(long long));
  return_if_malloc_failed(cnt);
  assign_const_I8(cnt, nT, 0); 

  bin_offset = malloc(nT * sizeof(long long));
  return_if_malloc_failed(bin_offset);

  t_cnt = malloc(nT * sizeof(long long *));
  return_if_malloc_failed(t_cnt);
  for ( int i = 0; i < nT; i++ ) { 
    t_cnt[i] = malloc(nT * sizeof(long long));
    return_if_malloc_failed(t_cnt[i]);
    assign_const_I8(t_cnt[i], nT, 0); 
  }
  t[1] = ippGetCpuClocks();

  // Figure out how many elements in each bin
  cilkfor ( int tid = 0; tid < nT; tid++ ) { 
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long *l_cnt = t_cnt[tid];
    for ( long long i = lb; i < ub; i++ ) { 
      unsigned int inval = (unsigned int) X[i];
      int bin = inval >> shift;
      if ( bin >= NUM_THREADS ) { status = -1; continue; }
      l_cnt[bin]++;
    }
  }
  t[2] = ippGetCpuClocks();
  // Sum up over local counts 
  for ( int i = 0; i < nT; i++ ) { 
    long long tempsum = 0;
    for ( int j = 0; j < nT; j++ ) { 
      tempsum += t_cnt[j][i];
    }
    cnt[i] = tempsum;
  }
  // Quick debugging check 
  long long chk_nX = 0;
  for ( int i = 0; i < nT; i++ ) { 
    chk_nX += cnt[i];
  }
  // Create cumulative counts
  bin_offset[0] = 0;
  for ( int i = 1; i < nT; i++ ) {
    bin_offset[i] = bin_offset[i-1] + cnt[i-1];
  }
  /*
  for ( int i = 1; i < nT; i++ ) {
    fprintf(stderr, "%d --> %lld \n", i, cnt[i]);
  }
  */
  t[3] = ippGetCpuClocks();

  if ( chk_nX != nR ) { go_BYE(-1); }
  // Now let us get elements to their correct bins
  for ( int i = 0; i < nR; i++ ) { 
    unsigned int inval = (unsigned int) X[i];
    int bin = inval >> shift;
    // if ( bin >= NUM_THREADS ) { status = -1; continue; }
    Y[bin_offset[bin]] = X[i];
    bin_offset[bin]++; 
  }
  cBYE(status);
  t[4] = ippGetCpuClocks();
  // Re-Create cumulative counts
  bin_offset[0] = 0;
  for ( int i = 1; i < nT; i++ ) {
    bin_offset[i] = bin_offset[i-1] + cnt[i-1];
  }

  // ------------------------------------------------------------
  // Now we can sort individual bins in parallel
  cilkfor ( int tid = 0; tid < nT; tid++ ) { 
    int *lY = (int *)Y; lY += bin_offset[tid];
    if ( cnt[tid] == 0 ) { continue; }
    if ( strcmp(mode, "asc") == 0 ) {
      qsort_asc_I4(lY, cnt[tid], sizeof(int), NULL);
    }
    else if ( strcmp(mode, "dsc") == 0 ) {
      qsort_dsc_I4(lY, cnt[tid], sizeof(int), NULL);
    }
    else { status = -1; continue; }
  }
  cBYE(status);
  t[5] = ippGetCpuClocks();
  /*
  for ( int i = 1; i < 5; i++ ) { 
    fprintf(stderr, "%d --> %lld \n", i, t[i] - t[i-1]);
  }
  */
  // ------------------------------------------------------------
 BYE:
  if ( t_cnt != NULL ) { 
    for ( int i = 0; i < nT; i++ ) { 
      free_if_non_null(t_cnt[i]);
    }
    free_if_non_null(t_cnt);
  }
  free_if_non_null(cnt);
  free_if_non_null(bin_offset);
  return(status);
}

#undef UNIT_TEST
#ifdef UNIT_TEST
/*
gcc -g -std=gnu99 -Wall auxil.c mmap.c _x.c ./AUTOGEN/GENFILES/assign_I*.c ./AUTOGEN/GENFILES/qsort_*I4.c -I./AUTOGEN/GENFILES -I. -lm
*/
#include <stdio.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "mmap.h"
#include "auxil.h"
#include "srt_uniform_I4.h"
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  long long nR;
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0;
  char *infile;
  long long someval, X_someval_cnt = 0, Y_someval_cnt = 0;
  if ( argc != 3 ) { go_BYE(-1); }
  infile = argv[1];
  status = stoI8(argv[2], &someval); 
  status = rs_mmap(infile, &X, &nX, 0); cBYE(status);
  nR = nX / sizeof(int);
  if ( ( nR * sizeof(int) ) != nX ) { go_BYE(-1); }
  Y = malloc(nX);
  return_if_malloc_failed(Y);
  status = srt_uniform_I4((int *)X, nR, (int *)Y, "asc"); cBYE(status);

  for ( long long i = 0; i < nR; i++ ) { 
    if ( ((int *)X)[i] == someval ) { X_someval_cnt++; }
    if ( ((int *)Y)[i] == someval ) { Y_someval_cnt++; }
  }
  if ( X_someval_cnt != Y_someval_cnt ) { go_BYE(-1); }
  fprintf(stderr, "%lld occurred %lld times \n", someval, X_someval_cnt);
BYE:
  munmap(X, nX);
  free_if_non_null(Y);
  return(status);
}
#endif
