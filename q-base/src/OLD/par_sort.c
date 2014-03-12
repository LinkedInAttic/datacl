#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "sort_asc_int.h"
#include "rs_qsort.h"

void
set_vals_int(
	     int X[],
	     int N
	     )
{
  for ( int i = 0; i < N; i++ ) { 
    X[i] = (int)lrand48();
  }
}

int
main()
{
  int status = 0;
 long long N, N_per_P;
#define NUM_THREADS 4
#define ITERS 1
  int *X = NULL; int *Y = NULL; int *Z = NULL;
  int *base = NULL;

  N = 33;

  X = (int *)malloc(N * sizeof(int));
  Y = (int *)malloc(N * sizeof(int));
  Z = (int *)malloc(N * sizeof(int));

  for ( int iters = 0; iters < ITERS; iters++ ) { 
    fprintf(stderr, "STARTED iter %d \n", iters);
    //---------------------------------------------------
    set_vals_int(X, N);
    for ( int i = 0; i < N; i++ ) { Y[i] = X[i]; }
    for ( int i = 0; i < N; i++ ) { Z[i] = X[i]; }
    //---------------------------------------------------
    _quicksort(X, N, sizeof(int), sort_asc_int);
    N_per_P = N / NUM_THREADS;
    for ( int p = 0; p < NUM_THREADS; p++ ) { 
      int n;
      base = Y + N_per_P;
      if ( p == NUM_THREADS - 1 ) { 
        n = N - (p*N_per_P);
      }
      else {
	n = N_per_P;
      }
      fprintf(stderr, "Sorting %d elements \n", n);
      _quicksort(base, n, sizeof(int), sort_asc_int);
    }
    //---------------------------------------------------
  }
  for ( int i = 0;  i < N; i++ ) { 
    fprintf(stderr, "%2d --> %10d: %10d \n", i, X[i], Y[i]);
  }
 BYE:
  free_if_non_null(X);
  free_if_non_null(Y);
  free_if_non_null(Z);
  return(status);
}
