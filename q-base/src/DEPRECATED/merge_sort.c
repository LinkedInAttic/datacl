#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"

int g_nT;
int *g_in;
int *g_scratch;
unsigned long long g_n;
unsigned long long g_binsz;
int *g_srt_ordr;

pthread_t g_threads[MAX_NUM_THREADS];
pthread_attr_t g_attr;
pthread_barrier_t g_barr; // Barrier variable

void *
core_merge_sort_asc_int(
    void *threadid
		   )
{
  int status = 0;
  int num_bins;
  long long xidx, yidx;
  int *X = NULL, *Y = NULL;
  unsigned long long n_out;
  int xval, yval;
  int *tmp_scratch = NULL;
  unsigned long long ybinsz; /* ybinsz is needed because 2 bins to merge
				may not have same size */

  int rc; 
  long tid = *((long *)threadid);
  int *in = g_in;
  int *scratch = g_scratch;
  unsigned long long n = g_n;
  unsigned long long binsz = g_binsz; 
  // assumption is that input is sorted in groups of binsz
  int *srt_ordr = g_srt_ordr;

  if ( binsz == 0 ) { go_BYE(-1); }
  if ( binsz >= n ) { goto BYE; }
  for ( ; ; ) { /* outer loop */
    if ( binsz >= n ) { goto BYE; } // all done 
    num_bins = n / binsz;
    if ( num_bins * binsz != n ) { num_bins++; } 
    // DEBUGGING for ( int i = 0; i < n; i++ ) { scratch[i] = INT_MAX; }
    for ( int b = 0; b < num_bins; b += 2 ) { /* process adjacent bins */
      if ( ( (b/2) % g_nT ) != tid ) { continue; } // not my work 
      if ( ( num_bins - b ) == 1 ) {  
	/* just one bin to process => nothing to do  */
        for ( int i = b*binsz; i < n; i++ ) { 
 	  scratch[i] = in[i];
        }
	break;
      }
      X = in + (b * binsz);
      tmp_scratch = scratch + (b * binsz);
      if ( ( (b+2) * binsz ) > n ) {
	ybinsz = n - ((b+1)*binsz);
      }
      else {
	ybinsz = binsz;
      }
      Y = X  + binsz;
      xidx = 0; 
      yidx = 0;
      n_out = 0;
      for ( ; n_out < binsz + ybinsz; ) {
	if ( ( xidx < binsz ) && ( yidx < ybinsz ) ) {
	  xval = X[xidx];
	  yval = Y[yidx];
	  if ( xval <= yval ) { 
	    tmp_scratch[n_out++] = xval;
	    xidx++;
	  }
	  else {
	    tmp_scratch[n_out++] = yval;
	    yidx++;
	  }
	}
	else if ( ( xidx < binsz ) && ( yidx >= ybinsz ) ) {
	  xval = X[xidx];
	  tmp_scratch[n_out++] = xval;
	  xidx++;
	}
	else if ( ( xidx >= binsz ) && ( yidx < ybinsz ) ) {
	  yval = Y[yidx];
	  tmp_scratch[n_out++] = yval;
	  yidx++;
	}
	else if ( ( xidx >= binsz ) && ( yidx >= ybinsz ) ) {
	  go_BYE(-1); /* control should not come here */
	}
      }
    }
    //-------------------------------------------------
    // Do the copy in blocks of size n/p 
    /* START: Sequential version 
    for ( int i = 0; i < n; i++ ) { 
      in[i] = scratch[i];
    }
    STOP: Sequential version  */
    // Synchronization point
    rc = pthread_barrier_wait(&g_barr);
    if ( (rc != 0) && (rc != PTHREAD_BARRIER_SERIAL_THREAD) ) {
      fprintf(stderr, "Could not wait on barrier\n"); go_BYE(-1);
    }
    //----------------------------------------
    int block_size = n / g_nT;
    if ( ( block_size * g_nT ) != g_nT ) { block_size++; }
    for ( int t = 0; t < g_nT; t++ ) { 
      if ( t != tid ) { continue; } // not my work 
      int lb = t * block_size;
      int ub = lb + block_size;
      if ( ub > n ) { ub = n; }
      for ( int j = lb; j < ub; j++ ) { 
	in[j] = scratch[j];
      }
    }
    // Synchronization point
    rc = pthread_barrier_wait(&g_barr);
    if ( (rc != 0) && (rc != PTHREAD_BARRIER_SERIAL_THREAD) ) {
      fprintf(stderr, "Could not wait on barrier\n"); go_BYE(-1);
    }
    //----------------------------------------
    binsz *= 2;
  }
 BYE:
  if ( g_nT == 1 ) {
    if ( status == 0 ) {
      return ((void *)0);
    }
    else {
      return ((void *)1);
    }
  }
  pthread_exit(NULL);
}

#define MIN_ROWS_FOR_PAR_SORT 8 // for debugging 
int
merge_sort_asc_int(
		   int *in, /* [n] */
		   int *scratch, /* [n] */
		   unsigned long long n, 
		   unsigned long long binsz, //  assumes that sorted in groups of binsz
		   int *srt_ordr
    )
{
  int status = 0;
  int nT = 8; // number of threads 
  int rc; // result code for thread create 
  void *thread_status;
  //--------------------------------------------
  g_in = in;
  g_scratch = scratch;
  g_n = n;
  g_binsz = binsz; 
  g_srt_ordr = srt_ordr;
  //--------------------------------------------
  if ( ( n <= MIN_ROWS_FOR_PAR_SORT ) || ( nT == 1 ) ) { 
    g_nT = nT = 1;
    void *vstat;
    vstat = core_merge_sort_asc_int(0);
  }
  else {
    g_nT = nT;
    /* Create threads: Initialize and set thread detached attribute */
    pthread_attr_init(&g_attr);
    pthread_attr_setdetachstate(&g_attr, PTHREAD_CREATE_JOINABLE);
    // Barrier initialization
    if ( pthread_barrier_init(&g_barr, NULL, nT)) {
      fprintf(stderr, "Could not create a barrier\n"); go_BYE(-1); 
    }
    for ( int t = 0; t < nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&g_threads[t], NULL, core_merge_sort_asc_int, 
	  (void *)&t);
      if ( rc ) {
	fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
	go_BYE(-1);
      }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&g_attr);
    for ( int t = 0; t < nT; t++ ) { 
      rc = pthread_join(g_threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
    }
  }
BYE:
  return(status);
}


#define STAND_ALONE
#ifdef STAND_ALONE
#include <stdlib.h>
#include <sys/time.h>
int
main()
{
  int status = 0;
  int *X = NULL;
  int *Y = NULL; // scratch pad
  int *ordr = NULL;
  struct timeval *Tps = NULL, *Tpf = NULL;
  void *Tzp = NULL;
  long long t_before_sec, t_before_usec, t_before;
  int N;

  //-------------------------------------------
  Tps = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tps == NULL ) { go_BYE(-1); }
  Tpf = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tpf == NULL ) { go_BYE(-1); }
  Tzp = 0;
  //-------------------------------------------
  gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
  t_before_sec  = (long long int)Tps->tv_sec;
  t_before_usec = (long long int)Tps->tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;
  srand48(t_before_usec);
  //-------------------------------------------
  for ( int iter = 0; iter < 32; iter++ ) { 
    N = ( lrand48() % 2*1048576 ) + 1 ;
    X = (int *)malloc(N * sizeof(int)); return_if_malloc_failed(X);
    Y = (int *)malloc(N * sizeof(int)); return_if_malloc_failed(Y);
    ordr = (int *)malloc(N * sizeof(int)); return_if_malloc_failed(ordr);
    for ( int i = 0; i < N; i++ ) { 
      X[i] = mrand48();
      ordr[i] = i;
    }
    status = merge_sort_asc_int(X, Y, N, 1, ordr);
    cBYE(status);
    for ( int i = 1; i < N; i++ ) { if ( X[i-1] > X[i] ) {go_BYE(-1); } }

    free_if_non_null(X);
    free_if_non_null(Y);
    free_if_non_null(ordr);
    fprintf(stderr, "Completed Iteration %d with %d elements \n", iter, N);
  }
BYE:
  free_if_non_null(Tps);
  free_if_non_null(Tpf);
  return(status);
}
 

#endif
