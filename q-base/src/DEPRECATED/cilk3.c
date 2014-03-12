#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <values.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/time.h>
#include <math.h>

// extern int gettimeofday(struct timeval *tv, struct timezone *tz);
extern long int random(void);

/* Purpose: Counts the number of occurrences of a set of integers in X,
 * assuming that each element of X is in the range [0, N-1] */

int
main(
    )
{
  // for timing
  struct timeval Tps; 
  struct timezone Tpf;
  long long t_before_sec = 0, t_before_usec = 0, t_before = 0;
  long long t_after_sec = 0, t_after_usec = 0, t_after = 0;

  // Allocate 2 arrays, X and Y
  int N = 64; int *Y = NULL; Y = malloc(N * sizeof(int)); 
  int M = 32*1048576; int *X = NULL; X = malloc(M * sizeof(int));
  // some fake data initialization 
  int val = 0;
  for ( int i = 0; i < M; i++ ) { X[i] = val; val++; if ( val == 2000 ) { val = 0; } } 
  for ( int i = 0; i < N; i++ ) { Y[i] = (random() % 1000) + 1000 ; }

  // Get the time before starting the Cilk loop
  gettimeofday(&Tps, &Tpf);
  t_before_sec  = (long long)Tps.tv_sec;
  t_before_usec = (long long)Tps.tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;

  // do the counting
#ifdef CILK
#define __FOR__ _Cilk_for
#else
#define __FOR__ for
#endif
  __FOR__ ( int i = 0; i < M; i++ ) { 
    int xval = X[i];
    int new_xval = -1;
    for ( int j = 0; j < N; j++ ) {
      if ( xval == Y[j] ) { 
	new_xval = j;
	break;
      }
    }
    X[i] = new_xval;
  }
  // Get the time after completing the Cilk loop
  gettimeofday (&Tps, &Tpf);
  t_after_sec  = (long long)Tps.tv_sec;
  t_after_usec = (long long)Tps.tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  // Print time 
  fprintf(stderr, "time = %lld \n", t_after - t_before);
}
