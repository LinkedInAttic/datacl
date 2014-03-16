#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "constants.h"
#include "macros.h"
#include "assign_I4.h"
#include "qsort_asc_I4.h"
#include "approx_quantile.h"
#include <malloc.h>
#include <fcntl.h>
#include "mmap.h"
#include "determine_b_k.h"
#include "New.h"
#include "Collapse.h"
#include "Output.h"

#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#endif

#define MAX_SZ 200*1048576
/* Will not use more than (4*200) MB of RAM, can change if you want */

// START FUNC DECL
int 
approx_quantile(
		int * x,
		char * cfld,
		long long siz,
		long long num_quantiles,
		double eps,
		int *y,
		long long y_siz,
		int *ptr_estimate_is_good
		)
// STOP FUNC DECL
//----------------------------------------------------------------------------
/* README:

status = approx_quantile(x,cfld, siz,num_quantiles,eps,y,y_siz,ptr_estimate_is_good): Calculates the approximate quantiles of an integer set using very little memory. 

For example: If you request for 10 quantiles with eps of 0.001: {10%, 20%,...90%, 100% } quantiles will be answered with an error of +/0.1% i.e., 10% quantile will be definitely between 9.9% and 10.1%, 90% quantile will definitely be between 89.9% and 90.1%.

Author: Kishore Jaganathan

Algorithm: Munro-Patterson Algorithm by G.S.Manku ("Approximate Medians and other Quantiles in One Pass with Limited Memory")

INPUTS: 

x: Array containing the input data to be processed.

cfld: two options - (1) NULL: All elements of x are processed.
(2) non-NULL: Array of same size as x. Acts as a select vector (only those elements with non-zero values in cfld are processed). ex: If x has 10 elements and cfld is {0,0,1,0,0,0,1,0,1,0}, then only the 3rd, 7th and 9th element are chosen for processing.

siz: Number of elements in the input array x. 

num_quantiles: Number of quantiles that have to be calculated (1 <= num_quantiles <= siz).  For ex: num_quantiles = 100 implies you need quantile queries every 1% from 1% to 100%, num_quantiles = 200 implies you need quantile queries every 0.5% from 0.5% to 100% and so on (try 100 if you don't know what to use).

eps: Acceptable error in the calculated quantiles (0 <= eps <= 1). For example, eps = 0.001 would imply +/- 0.1%, eps = 0.0001 would imply +/- 0.01% (try 0.001 if you don't know what to use).

y_siz: Number of integers that can be written in y (preallocated memory). Has to be atleast num_quantiles*sizeof(int).

OUTPUTS: 

y: Array where the quantile summary is going to be stored, need to malloc with y_siz integer memory (>= num_quantiles*sizeof(int)) beforehand.

ptr_estimate_is_good: Pointer to the location with values 1, -1 or -2, which stand for the following
1: For the given inputs siz and eps, approximate quantile calculations are possible. The computations are done and results are stored in y.
-1: Something wrong with the inputs.
-2: For the given inputs siz and eps, approximate quantile calculations are not possible within the memory constraints. Retry with: 
(i) a higher value for MAX_SZ defined in this function if you know you have more RAM available 
(ii)a higher value for eps (i.e., more approximation) so that the computations can be done in memory 

status: takes values -1 or 0
0: The algorithm either computed the quantiles and set *ptr_estimate_is_good to 1 or the computations are not possible for the given eps and siz and set *ptr_estimate_is_good to -2.
-1: Something wrong with the inputs, *ptr_estimate_is_good is set to -1.
 
*/

//----------------------------------------------------------------------------
{

  int status = 0;
  
  int flag = 0; /* used to assist freeing of mallocs */

  *ptr_estimate_is_good = -1; /* default */

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( y == NULL ) { go_BYE(-1); }
  if ( ( eps < 0 )  || ( eps > 1 ) ) { go_BYE(-1); }
  if ( num_quantiles < 1 ) { go_BYE(-1); }
  if ( siz <= 0 ) { go_BYE(-1); }
  if ( y_siz < num_quantiles ) { go_BYE(-1); } /* insufficient memory to write output */

  long long eff_siz = 0; /* number of entries to be considered */
  if ( cfld == NULL ) { eff_siz = siz; }
  else {

    for ( long long ii = 0; ii < siz; ii++ ) {
      if ( cfld[ii] == 0 ) { continue; }
      eff_siz++;
    }

    if ( eff_siz == 0 ) { go_BYE(-1); } /* cfld has all 0 entries */

  }

  //-------------------------------------------------------------------------

  /* "buffer" is a 2d array containing b buffers, each of size k. Each of these b buffers have a weight assigned to them, which will be stored in "weight" array of size b. Consider the following way of viewing the 2d buffer array: each element in a buffer "effectively" occurs as many times as it's corresponding weight in the weight array. The algorithm  compresses the whole input data into these buffers by using "approximate" entries instead of actual entries so that the total number of distinct entries comes down significantly (uses a total memory of ~ b*k, which is typically << eff_siz, the price paid being approximation). This approximation is done intelligently so that very good and useful theoretical quantile guarantees can be provided */

 
  int b;
  long long k; 
  status = determine_b_k(eps, eff_siz, &b, &k);  cBYE(status);
  /* estimates b and k for the given eps and eff_siz */
  
  int NUM_THREADS; 
  /* explained in the next section, mainly to allow parallelizable computations to be done in parallel */

  if ( b <= 0 || k <= 0 ) {
    *ptr_estimate_is_good = -1;
    go_BYE(-1); /* Something wrong with the inputs eps or siz */ 
  }
  else if ( (b+1+10)*k > MAX_SZ ) {

    /* (b+1+10)*k a good upper bound of the memory requirements */
    *ptr_estimate_is_good = -2; 
    go_BYE(0);
    /* Quitting if too much memory needed. Retry by doing one or more of the following: 
     (i) Increase MAX_SZ if you think you have more RAM 
     (ii) Increase eps (the approximation percentage) so that computations can be done within RAM
    */
  } 
  else {
    *ptr_estimate_is_good = 1;

    NUM_THREADS = 128;
    while ( (b+NUM_THREADS+10)*k > MAX_SZ ) { NUM_THREADS = NUM_THREADS/2; }
    /* adapting NUM_THREADS to meet memory requirements */
    
  }

  int **buffer = NULL;         
  int *weight = NULL;  

  flag = 1; /* buffer and weight defined */

  int no_of_empty_buffers = b; /* no of free buffers in the 2d buffer array*/

  buffer      = malloc( b * sizeof(int *) ); 
  return_if_malloc_failed(buffer);

  weight      = malloc( b * sizeof(int) ); 
  return_if_malloc_failed(weight);
#ifdef IPP
  ippsZero_32s((int *)weight,b);
#else
  assign_const_I4(weight,b,0);
#endif

  for ( int ii = 0; ii < b; ii++ ) {
    buffer[ii] = (int *) malloc( k * sizeof(int) );
  }

  flag = 2; /* buffer[ii] defined for ii = 0 to b-1 */
  
  for ( int ii = 0; ii < b; ii++ ) {
    return_if_malloc_failed(buffer[ii]);
#ifdef IPP
    ippsZero_32s((int *)buffer[ii], k);
#else
    assign_const_I4(buffer[ii], k, 0); 
#endif

  } 

  //--------------------------------------------------------------------------

  /* The Munro-Patterson algorithm assumes that the incoming data is in the form of packets of size k with sorted data. Since the sorting has to be done within each packet separately, we can parallelize this step as follows: we divide the incoming data into blocks of size  NUM_THREADS*k (so that NUM_THREADS threads can be generated for each block and sorted separately in parallel using cilkfor). */

  /* "inputPacket" is a 2d array of size NUM_THREADS * k: stores and sorts packets belonging to the same block in parallel using cilkfor. Since the last packet might be incomplete, it will be dealt with separately, using "lastPacket" if k does not divide eff_siz. Variable last_packet_incomplete will be used to keep track of this */

  int **inputPackets = NULL; 
  int *lastPacket = NULL;
  long long * inputPacketsUsedSiz = NULL;

  flag = 3; /* inputPackets, inputPacketsUsedSiz and lastPacket defined */

  inputPackets = malloc( NUM_THREADS * sizeof(int *) );
  return_if_malloc_failed(inputPackets); 
  
  inputPacketsUsedSiz = malloc( NUM_THREADS * sizeof(long long) );
  return_if_malloc_failed(inputPacketsUsedSiz);

  for ( long long ii = 0; ii < NUM_THREADS; ii++) {
    inputPacketsUsedSiz[ii] = 0;
  }

  lastPacket = (int *)malloc ( k * sizeof(int));
  return_if_malloc_failed(lastPacket);
#ifdef IPP
  ippsZero_32s((int *)lastPacket,k);
#else
  assign_const_I4(lastPacket,k,0);
#endif

  long long lastPacketUsedSiz = 0;
  
  for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
    inputPackets[ii] =  (int *) malloc( k * sizeof(int) );
  }

  flag = 4; /* inputPackets[ii] defined for ii = 0 to NUM_THREADS-1 */

  for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
    return_if_malloc_failed(inputPackets[ii]);
#ifdef IPP
    ippsZero_32s((int *)inputPackets[ii],k);
#else
    assign_const_I4(inputPackets[ii],k,0);
#endif
  }

  //---------------------------------------------------------------------------
  
  long long current_loc_in_x = 0; /* start of input data */
  int last_packet_incomplete = 0; 

  /* Do the following for each block of data */
  while ( current_loc_in_x < siz ) { 

    /* A block of data (containing NUM_THREADS packets, i.e., NUM_THREADS * k integers) is processed inside this loop. For each packet, the following operations are done:
     (1): Sort the packet (can be done in parallel using cilkfor)
     (2): Check if a buffer in the 2d buffer array is free (i.e., some buffer has weight = 0 )
     (3): If yes, copy the packet to a free buffer in the buffer array using New() function. Else, use Collapse() function to merge two buffers in the buffer array which have the same weight and free up one buffer in the process (and copy the packet to that buffer)
    */

    if ( cfld == NULL || eff_siz == siz ) {
      //---------------------------------------------------------------------
      /* considering all input data */

      for ( long long ii = 0; ii < NUM_THREADS; ii++) {
	inputPacketsUsedSiz[ii] = 0;
      }
      
      cilkfor ( int tid = 0; tid < NUM_THREADS; tid++ ) {

	long long lb = current_loc_in_x + tid *k;
	long long ub = lb + k;
	if ( lb >= siz ) { continue; }
	if ( ub >= siz ) { 

	  ub = siz;  
	  if ( (ub-lb) != k ) {
	    /* this happens when last packet is incomplete */
	    memcpy(lastPacket, x+lb, (ub-lb)*sizeof(int));
	    lastPacketUsedSiz = (ub-lb);
	    last_packet_incomplete = 1;
	  }
	  else {
	    /* last packet is also complete: eff_siz is multiple of k */
	    memcpy(inputPackets[tid], x+lb, (ub-lb)*sizeof(int));
	    inputPacketsUsedSiz[tid] = (ub-lb);
	  }
	  continue;
	 
	}

	memcpy(inputPackets[tid], x+lb, (ub-lb)*sizeof(int));
	inputPacketsUsedSiz[tid] = (ub-lb);

      }

      for ( int tid = 0; tid < NUM_THREADS; tid++ ) {
	current_loc_in_x += inputPacketsUsedSiz[tid];
      }
      current_loc_in_x += lastPacketUsedSiz;
      //---------------------------------------------------------------------
    }

    else {
      //--------------------------------------------------------------------
      /* NOTE: if cfld input is non-null, it means we are not interested in all the elements. In every iteration, we keep filling inputPackets buffer with only those data we are interested in using the helper variable "current_loc_in_x". */
      
      int tid = 0;
      for ( long long ii = 0; ii < NUM_THREADS; ii++ ) {
	inputPacketsUsedSiz[ii] = 0;
      }
      
      while ( current_loc_in_x < siz  && tid < NUM_THREADS ) {

	if ( cfld[current_loc_in_x] == 0 ) { current_loc_in_x++; }
	else {
	  inputPackets[tid][inputPacketsUsedSiz[tid]]=x[current_loc_in_x];
	  current_loc_in_x++; inputPacketsUsedSiz[tid]++;
	  if ( inputPacketsUsedSiz[tid] == k ) { tid++; }
	}

      }

      if ( current_loc_in_x == siz ) {
	
	for ( int ii = 0; ii <= NUM_THREADS; ii++ ) {
	  if ( inputPacketsUsedSiz[tid]!=0 && inputPacketsUsedSiz[tid]!=k ) { 
	    last_packet_incomplete = 1; 
	    memcpy(lastPacket, inputPackets[tid], inputPacketsUsedSiz[tid]*sizeof(int));
	    lastPacketUsedSiz = inputPacketsUsedSiz[tid];
	    inputPacketsUsedSiz[tid] = 0;
	    break; 
	  }
	}

      }

      //--------------------------------------------------------------------
    }

    /* Step (1) done here in parallel using cilkfor */
    cilkfor ( int tid = 0; tid < NUM_THREADS; tid++ ) { 

      if ( inputPacketsUsedSiz[tid] != k ) { continue; }

#ifdef IPP
      ippsSortAscend_32s_I(inputPackets[tid], inputPacketsUsedSiz[tid]);
#else
      qsort_asc_I4(inputPackets[tid], inputPacketsUsedSiz[tid], sizeof(int), NULL);
#endif 

    }
  
    /* Steps (2) and (3) of the algorithm done here */
    for ( int tid = 0; tid < NUM_THREADS; tid++ ) {

      if ( inputPacketsUsedSiz[tid] != k ) { continue; }

      if ( no_of_empty_buffers == 0 ) {

	/* if no free buffer available in the 2d buffer array , merge data in 2 buffers having same weight into one of them using Collapse() and free up other */
	bool found = false;
	int bufidx1 = -1, bufidx2 = -1; 
	for ( int ii = 0; ii < b-1; ii++ ) { 
	  for ( int jj = ii+1; jj < b; jj++ ) {
	    if ( weight[ii] == weight[jj] && weight[ii] > 0 ) { 
	      bufidx1 = ii; bufidx2 = jj;
	      found = true;
	      break;
	    }
	  }
	  if ( found == true ) { break; }
	} /* find 2 buffers with same corresponding weight in the weight array */
	status = Collapse(buffer[bufidx1],buffer[bufidx2], weight, bufidx1, bufidx2, b, k);  /* Merge buffer numbers [bufidx1] and [bufidx2] */
	if ( status == -1 ) { 
	  *ptr_estimate_is_good = -1;
	  go_BYE(-1);  /* something fundamentally wrong */
	}
	no_of_empty_buffers++;

      }

      bool found = false;
      int bufidx1 = -1;
      for ( int ii = 0; ii < b; ii++ ) {
	if ( weight[ii] == 0 ) {
	  no_of_empty_buffers--;
	  found = true;
	  bufidx1 = ii;
	  break;
	}
      } /* find a free buffer (corresponding weight = 0 in the weight array) */
      status = New(inputPackets[tid],buffer[bufidx1],weight,1,bufidx1,b,k);
      if ( status == -1 ) {
	*ptr_estimate_is_good = -1;
	go_BYE(-1);  /* something fundamentally wrong */
      } 
      /* Copy current input packet into a free buffer in the 2d buffer array*/


    }

  }
  //---------------------------------------------------------------------------
  
  /* Deal with the last packet separately here if it is not full */

  if ( last_packet_incomplete == 1 ) {
#ifdef IPP
    ippsSortAscend_32s_I(lastPacket, lastPacketUsedSiz);
#else
    qsort_asc_I4(lastPacket, lastPacketUsedSiz, sizeof(int), NULL);
#endif
  }

  //--------------------------------------------------------------------------

  /* Final quantile computations using data from 2d buffer array, weight array and last packet (if it exists) */

  status = Output(buffer,weight,lastPacket, last_packet_incomplete, lastPacketUsedSiz,eff_siz,num_quantiles,y,b,k); 
  if ( status == -1 ) {
    *ptr_estimate_is_good = -1;
    go_BYE(-1); /* something fundamentally wrong */
  }

  //--------------------------------------------------------------------------

 BYE:
  
  if ( flag >= 4 ) {
    for ( int ii = 0; ii < NUM_THREADS; ii++ ) {
      free_if_non_null(inputPackets[ii]);
    }
  }

  if ( flag >= 3 ) {
    free_if_non_null(inputPackets);
    free_if_non_null(lastPacket);
    free_if_non_null(inputPacketsUsedSiz);
  }

  if ( flag >= 2 ) {    
    for ( int ii = 0; ii < b; ii++ ) {
      free_if_non_null(buffer[ii]);
    }
  }

  if ( flag >= 1) {
    free_if_non_null(buffer);
    free_if_non_null(weight);
  }
 
  return(status);

}
