#include <stdio.h>
#include "stdlib.h"
#include "constants.h"
#include "macros.h"
// START FUNC DECL 
int
get_outer_lb_ub(
		long long nR,
		int nT,
		int t, /* thread index */
		long long *ptr_lb,
		long long *ptr_ub
		)
// STOP FUNC DECL 
{
  int status = 0;
  long long nRprime; /* modified nR so that division has no remainder */
  long long nR_per_thread; /* number of rows for each thread to work on */

  if ( nR < 1 ) { go_BYE(-1); }
  if ( nT < 1 ) { go_BYE(-1); }
  /* Block size for each thread */
  if ( ( ( nR / nT ) * nT ) != nR ) {
    nRprime = nR + nT;
  }
  else {
    nRprime = nR;
  }
  nR_per_thread = nRprime / nT;
  //--------------------------------------------
  *ptr_lb = nR_per_thread * t;
  *ptr_ub = *ptr_lb + nR_per_thread;
  if ( *ptr_ub > nR ) { *ptr_ub = nR; }
 BYE:
  return(status);
}

// START FUNC DECL 
int
get_num_blocks(
	       long long nR, /* number of elements to process */
	       int nC,  /* number of elements to process in one block */
	       long long *ptr_nB
	       )
// STOP FUNC DECL 
{
  int status = 0;
  long long nRprime; /* modified nR so that division has no remainder */

  if ( nR < 1 ) { 
    go_BYE(-1); }
  if ( nC < 1 ) { go_BYE(-1); }

  if ( ( ( nR / nC ) * nC ) != nR ) {
    nRprime = nR + nC;
  }
  else {
    nRprime = nR;
  }
  *ptr_nB = nRprime / nC;
 BYE:
  return(status);
}
// START FUNC DECL
int
get_num_threads(
    int *ptr_nT
    )
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL;
  if ( getenv("Q_NUM_THREADS") == NULL ) { 
    *ptr_nT = DEFAULT_NUM_THREADS; 
  }
  else {
    *ptr_nT = strtol(getenv("Q_NUM_THREADS"), &endptr, 10);
  }
  if ( ( *ptr_nT < 1 ) || ( *ptr_nT > MAX_NUM_THREADS ) ) { go_BYE(-1); }
  // fprintf(stderr, "Using %d threads \n", *ptr_nT);
BYE:
  return(status);
}
