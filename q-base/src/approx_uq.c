#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "assign_I4.h"
#include "spooky_hash.h"
#include "qsort_asc_I4.h"
#include "determine_rho_loc.h"
#include "approx_uq.h"

#define MIN_SZ 100000

// START FUNC DECL
int 
approx_uq(
	  int *x, /* input data stream */ 
	  char *cfld, /* conditional selector */
	  long long siz, /* input data size */
	  long long *y /* output location: store no of unique elements*/
	  )
// STOP FUNC DECL
{

  /* Refer to HyperLogLog paper of Flajolet for algorithm details */

  int status = 0;
  uint64_t *hashval = NULL;
  int *srt_x = NULL;

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( y == NULL ) { go_BYE(-1); }
  if ( siz < MIN_SZ ) { go_BYE(-1); }
  //--------------------------------------------------------------
  /* m = number of bins: use power of 2, 128 <= m <= 2^24. Code needs to 
   * be changed if you need to use higher m, not recommended */
  int m = 4096; 
  if ( (m & (m-1)) != 0 || m < 128 ) { go_BYE(-1); }
  int *max_rho = NULL;  /* stores maximum rho value for each bin */
  double temp_val = 0;  /* used for intermediate computations */
  int rho = 0, loc = 0; /* stores intermediate values (rho and  bin loc)*/
  uint64_t seed = time(NULL);
#define BLOCK_SIZE 1048576
  hashval = malloc(BLOCK_SIZE * sizeof(uint64_t) );
  return_if_malloc_failed(hashval); 
  for ( int ii = 0; ii < BLOCK_SIZE; ii++ ) { 
    hashval[ii] = 0; 
  }
  // Allocate memory for variables 
  max_rho = malloc(m*sizeof(int));
  return_if_malloc_failed(max_rho);
  // Initialize
  for ( int ii = 0; ii < m; ii++ ) {
    max_rho[ii] = 0;
  }
  //--------------------------------------------------------------
  /* For each element in the input data stream: calculate a hash value
     using spooky_hash64, determine the rho and loc (bin id) values
     using determine_rho_loc and update max_rho accordingly */

  int num_blocks = siz / BLOCK_SIZE;
  if ( ( num_blocks * BLOCK_SIZE != siz ) ) {
    num_blocks++;

  }
  for ( int b = 0; b < num_blocks; b++ ) { 
    long long lb = b * BLOCK_SIZE;
    long long ub = lb + BLOCK_SIZE;
    if ( ub > siz )  { ub = siz; }
    if ( ( ub - lb ) > BLOCK_SIZE ) { go_BYE(-1); }
    // parallel computation of hash values 
    if ( cfld == NULL ) { 
#pragma cilk grainsize = 64
      cilkfor ( long long ii = 0 ; ii < (ub-lb) ; ii++ ) {
	if ( ( cfld != NULL ) && ( cfld[lb+ii] == 0 ) ) { continue; }
	int t_value[2]; 
	t_value[0] = x[lb+ii];
	t_value[1] = 0; 
	/* TODO: Check whether +1 needed below for strlen */
	hashval[ii] = spooky_hash64((char *)t_value, sizeof(int), seed);
      }
      for ( long long ii = 0; ii < (ub-lb) ; ii++ ) {
	// determine_rho_loc() could be put in the Cilkfor loop. TODO P3
	status = determine_rho_loc(hashval[ii], &rho, &loc, m); cBYE(status);
	if ( max_rho[loc] < rho ) { 
	  max_rho[loc] = rho;
	}
      }
    }
  }
  //----------------------------------------------------------------
  /* final computations (once max_rho is determined), see Flajolet's
     Hyperloglog to understand the expression */

  for ( int ii = 0; ii < m; ii++ ) {
    temp_val = temp_val + pow(2,-(double)max_rho[ii]);
  }
  if ( temp_val > 0) {
    temp_val = 1/temp_val;
  }
  else {
    go_BYE(-1);
  }

  *y = (int)(0.721*pow(m,2)*temp_val);

 BYE:
  free_if_non_null(hashval);
  return (status);
}
