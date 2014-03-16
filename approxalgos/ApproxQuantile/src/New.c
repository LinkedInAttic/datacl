#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include "constants.h"
#include "macros.h"
#include "New.h"
#include <malloc.h>
#include <fcntl.h>
#include <string.h>

// START FUNC DECL
int New(
	int *src_buf, 
	int *dst_buf,  
	int *ptr_weight,  
	int initial_weight, 
	int bufidx,  
	int b,  
	long long k 
	)
// STOP FUNC DECL
//---------------------------------------------------------------------------
/* README:

New(src_buf, dst_buf, ptr_weight,initial_weight,bufidx,b,k): This function takes an input packet of size k with sorted data and stores it in one of the unused buffers in the 2d buffer array 

(i) you need to ensure beforehand that the buffer you are writing to is free, i.e., it's weight is 0 
(ii) you need to sort the input packet beforehand 

NOTE: New() function in G.S. Manku's paper "Approximate Medians and other Quantiles in One Pass with Limited Memory" -- section: Munro-Patterson algorithm 

NOTE: Consider the following way of viewing the buffer array: each element in a buffer occurs as many times as it's corresponding weight in the weight array. The New() operation adds ( initial_weight * k ) "effective" elements to the 2d buffer array (most algorithms use initial_weight 1, so that New() adds k "effective" elements for an input packet of size k).

INPUTS:

src_buf: Input packet of size k containing sorted data, where k is the size of each buffer in the 2d buffer array

dst_buf: Pointer to the free buffer in the 2d buffer array where the input sorted packet is going to be written

ptr_weight: Array containing the weight information of each buffer in the 2d buffer array

initial weight: Weight to be assigned to the buffer with the input packet (1 for Munro-Patterson Algorithm)

bufidx: Id of the buffer in the 2d buffer array which is going to store the input packet

b: Number of buffers in the 2d buffer array (produced by determine_b_k)

k: Size of each buffer in the 2d buffer array (produced by determine_b_k)

 */
//---------------------------------------------------------------------------
{

  int status = 0;

  /* check inputs: something fundamentally wrong if this exits here */ 
  if ( src_buf == NULL ) { go_BYE(-1); }
  if ( dst_buf == NULL ) { go_BYE(-1); }
  if ( ptr_weight == NULL ) { go_BYE(-1); }
  if ( bufidx < 0 || bufidx >= b ) { go_BYE(-1); } 
  /* buffer id out of range */

  memcpy(dst_buf, src_buf, k * sizeof(int));
  ptr_weight[bufidx] = initial_weight ; 

 BYE:
  return(status);
}
