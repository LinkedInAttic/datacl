// compile as gcc sample_openmp.c -O4 -lm -std=gnu99 -fopenmp -lgomp
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include "macros.h"


int
main()
{
  long long *X = NULL;
#define N (16*1048676)

  X = malloc(N * sizeof(double));
  for ( long long i = 0; i < N; i++ ) {
    X[i] = i;
  }
  int nT = 64;
  int block_size = N / nT;
  omp_set_num_threads(8);
#pragma omp parallel  for
  for ( int tid = 0; tid < nT; tid++ ) { 
    int lb = tid * block_size;
    int ub = lb  + block_size;
    for ( int j = lb; j < ub; j++ ) {
      X[j] = sin(log((j+1)));
    }
  }
BYE:
  free_if_non_null(X);
}

