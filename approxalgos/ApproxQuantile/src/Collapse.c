#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include "constants.h"
#include "macros.h"
#include "Collapse.h"
#include <malloc.h>

// START FUNC DECL
int 
Collapse(
	 int *buf1,   
	 int *buf2,  
	 int *ptr_weight, 
	 int bufidx1,
	 int bufidx2,  
	 int b,        
	 long long k  
	 )
// STOP FUNC DECL
//---------------------------------------------------------------------------
/* README: 

Collapse(buf1,buf2,ptr_weight,bufidx1,bufidx2,b,k): This function takes two buffers in the 2d buffer array with weights strictly greater than 0 and collapses them to form one buffer of size k (by throwing away some elements "intelligently"). It stores the merged information in the first of the two buffers, and frees up the other buffer for new input packets.

NOTE: Collapse() function in G.S. Manku's paper "Approximate Medians and other Quantiles in One Pass with Limited Memory" -- section: Munro-Patterson Algorithm

NOTE: Consider the following way of viewing the buffer array: each element in a buffer occurs as many times as it's corresponding weight in the weight array. The Collapse() operation doesn't alter the total number of "effective" elements in the buffer array.

INPUTS: 

buf1: Pointer to the buffer in the 2d buffer array containing the first buffer to be merged

buf2: Pointer to the location in the 2d buffer array containing the second buffer to be merged

ptr_weight: Array containing the weight information of each buffer in the 2d buffer array. ptr_weight[bufidx1] will become ptr_weight[bufidx1]+ptr_weight[bufidx2] after merging, and ptr_weight[bufidx2] will become 0.

bufidx1: Id of the first buffer in the 2d buffer array, also the location where the merged buffer will be stored

bufidx2: Id of the second buffer in the 2d buffer array, will become free at the end of this operation

b: Number of buffers in the 2d buffer array (produced by determine_b_k)

k: Size of each buffer in the 2d buffer array (produced by determine_b_k)

 */
//---------------------------------------------------------------------------
{

  int status = 0;

  int flag = 0; /* used to assist freeing of mallocs */

  /* check inputs: something fundamentally wrong if it exits here  */ 
  if ( buf1 == NULL ) { go_BYE(-1); } 
  if ( buf2 == NULL ) { go_BYE(-1); }
  if ( ptr_weight == NULL ) { go_BYE(-1); }
  if ( bufidx1 < 0 || bufidx1 >= b ) { go_BYE(-1); } /* out of range id */
  if ( bufidx2 < 0 || bufidx2 >= b ) { go_BYE(-1); }
  if ( ptr_weight[bufidx1] <= 0 || ptr_weight[bufidx2] <= 0 ) { go_BYE(-1); }

  /* Step (1): "tempBuffer" of size 2*k will be used to merge the two sorted arrays buf1 and buf2 of size k into a single sorted array. The weight information of each element in tempBuffer will be stored in tempWeight.

     For example: if k=5, buf1 = {1,3,5,7,9} with corresponding weight = 1 and buf2= {2,4,6,8,10} with corresponding weight = 2. tempBuffer will be {1,2,3,4,5,6,7,8,9,10} with tempWeight being {1,2,1,2,1,2,1,2,1,2} */

  int* tempBuffer = NULL;
  int* tempWeight = NULL;

  flag = 1; /* tempWeight and tempBuffer have been defined */

  tempBuffer = malloc( 2*k * sizeof(int) ); 
  return_if_malloc_failed(tempBuffer);
  tempWeight = malloc( 2*k * sizeof(int) ); 
  return_if_malloc_failed(tempWeight);

  long long ii = 0, jj = 0, kk = 0; 

  while(1) {

    if ( ii < k && jj < k ) {
      if ( buf1[ii] <= buf2[jj] ) {
	tempBuffer[kk] = buf1[ii++];
	tempWeight[kk++] = ptr_weight[bufidx1];
      }
      else {
        tempBuffer[kk] = buf2[jj++];
	tempWeight[kk++] = ptr_weight[bufidx2];
      }
    }

    else if ( ii < k && jj == k ) {
      tempBuffer[kk] = buf1[ii++];
      tempWeight[kk++] = ptr_weight[bufidx1];
    }

    else if ( ii == k && jj < k ) {
      tempBuffer[kk] = buf2[jj++];
      tempWeight[kk++] = ptr_weight[bufidx2];
    }

    else
      break;

  }

  /* We are left with an array of 2*k elements with varying weights. We need to pick k elements that "best" represent this set and store it in bufidx1 buffer of the 2d buffer array. */

  /* This is done as follows: Suppose we view tempBuffer as a set where each element occurs as many times as it's corresponding tempWeight entry. The total number of elements is (ptr_weight[bufidx1]+ptr_weight[bufidx2])*k (the elements are sorted too). Partition this set into k sets of equal size ( = (ptr_weight[bufidx1]+ptr_weight[bufidx2]) ) and pick the middle element from each partition. */

  /* For example: if tempBuffer = {1,2,3,4,5,6,7,8,9,10} and tempWeight = {1,2,1,2,1,2,1,2,1,2}. The partition would look like {1,2,2}{3,4,4}{5,6,6}{7,8,8}{9,10,10}. We will hence pick {2,4,6,8,10} which are the middle entries in each of the partition and add that set to bufidx1.

     NOTE: We don't have to do this expansion explicitly. It will be done intelligently without using any additional memory in the following steps using counters ii,jj and kk. */


  ii = 0; jj = 0; kk = 0;

  int outWeight = ptr_weight[bufidx1]+ptr_weight[bufidx2]; 
  /* size of each partition */

  int offSet = outWeight/2;
  /* offset is used to pick the middle element of each partition */
  
  while ( ii < 2*k ) {
    
    kk += tempWeight[ii];

    while ( kk >= offSet+jj*outWeight ) {
      buf1[jj++] = tempBuffer[ii];
    }

    ii++;

  }

  /* Update the weight array after Collapse operation */

  ptr_weight[bufidx1] = outWeight;
  ptr_weight[bufidx2] = 0;

  
 BYE:

  if ( flag == 1 ) {
    free_if_non_null(tempBuffer);
    free_if_non_null(tempWeight);
  }

  return(status);

}
