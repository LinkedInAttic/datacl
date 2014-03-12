#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "sort_asc_int.h"

int
merge_sort_asc_I(
		 int *in_X,
		 long long nB, /* number in bottom half */
		 long long nT /* number in top    half */
		 )
{
  int status = 0;
  int *X; /* points to where we need to write */
  int *T; /* points to top half */
  int *B; /* points to bottom half */
  int *O; /* points to overflow of top half */
  long long nO;
  long long nX; /* number of items to write */
  long long in_nX; /* number of items */
  int b; /* bottom value to compare */
  int t; /* top    value to compare */
  int x; /* value about to be over-written */
  bool fromO;
  int i, j; char c;

  X = in_X;
  B = X;
  T = X + nB;
  O = T; /* not sure if this is correct */
  nO = 0;
  nX = nT + nB;
  in_nX = nX;

  for ( j = 0; nX > 0; j++ ) {
    for ( i = 0; i < in_nX; i++ ) { 
      if ( i < j ) {
	c = 'X';
      }
      else if ( ( B - in_X ) == i ) { 
	  c = 'B';
	}
	else if ( ( T - in_X ) == i ) { 
	  c = 'T';
	}
	else if ( ( O - in_X ) == i ) { 
	  c = 'O';
	}
	else {
	  c = ' ';
	}
      fprintf(stdout, "%c: %3d: %d \n", c, i, in_X[i]);
    }
    fprintf(stdout, "--------------- %d -------------\n", nX);
    fprintf(stdout, "B = %lld, nB = %lld \n", B - in_X, nB);
    fprintf(stdout, "T = %lld, nT = %lld \n", T - in_X, nT);
    fprintf(stdout, "O = %lld, nO = %lld \n", O - in_X, nO);
    fprintf(stdout, "X = %lld, nX = %lld \n", X - in_X, nX);
    if ( ( nB + nO + nT ) != nX ) { 
      fprintf(stderr, "bad \n");
	  go_BYE(-1); }
    if ( nT == 0 ) {
      // Nothing in top half to work on
      // Need to swap B and O. Everything in 0 <= everything in B
      break;
    }
    t = *T;
    fromO = false;
    if ( nO == 0 ) {
      if ( nB == 0 ) {
	// Nothing in bottom half to work on
	break;
      }
      b = *B;
    }
    else {
      b = *O;
      fromO = true;
    }
    /* Now we have the values we need to compare */
    if ( b <= t ) {  /* consume the bottom value */
      if ( fromO ) { 
	x = *X;
	*X = b; 
	*O = x;
	O++;
	nO--;
	nB++; /* because we will decrement nB later on */
      }
    }
    else { /* consume the top value */
      x  = *X;
      *X = t;
      *T = x; /* bottom value goes to overflow on top */
      /* CHECK IS ABOVE ALWAYS THE CASE ? */
      nO++;
      T++;
      nT--;
    }
    X++;
    nX--;
    B++;
    nB--;
  }

 BYE:
  return(status);
}


int
main()
{
  int status = 0;
  struct timeval Tps;
  struct timezone Tpf;
  long long t_before_sec ;
  long long t_before_usec;
  long long t_before ;
  int i;

  status= gettimeofday(&Tps, &Tpf); cBYE(status);
  t_before_sec  = (long long)Tps.tv_sec;
  t_before_usec = (long long)Tps.tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;
  srand48(t_before);


#define N 17
  int *X = NULL;
  X = (int *)malloc(N * sizeof(int));
  for ( i = 0; i < N; i++ ) { 
    X[i] = mrand48() % 1000;
  }
X[0] =  -956;
X[1] =  -237;
X[2] =  -146;
X[3] =  -63;
X[4] =  122;
X[5] =  182;
X[6] =  330;
X[7] =  728;
X[8] =  -858;
X[9] =  -943;
X[10] =  -643;
X[11] =  -558;
X[12] =  -315;
X[13] =  -255;
X[14] =  16;
X[15] =  337;
X[16] =  364;
  qsort(X, N/2, sizeof(int), sort_asc_int);
  qsort(X+N/2, N - N/2, sizeof(int), sort_asc_int);

  status = merge_sort_asc_I(X, N/2, N - N/2);

  for ( i = 0; i < N; i++ ) { 
    fprintf(stdout, "%3d: %d \n", i, X[i]);
  }

BYE:
  free_if_non_null(X);
  return(status);
}
