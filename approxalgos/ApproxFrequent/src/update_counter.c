#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "macros.h"

// START FUNC DECL
int 
update_counter (
		int * cntr_id,   
		int * cntr_freq, 
		long long cntr_siz, 
		long long *ptr_active_cntr_siz,  
		int * bf_id,      
		int * bf_freq,      
		long long bf_siz   
		)
// STOP FUNC DECL
//-------------------------------------------------------------------------
/* README:

update_counter(cntr_id,cntr_freq,cntr_siz,ptr_active_cntr_siz,bf_id,bf_freq,bf_siz) : This function updates the counter array (cntr_id, cntr_freq) by adding new id and freq data specified by (bf_id, bf_freq).

NOTE: Both (cntr_id,cntr_freq) and (bf_id,bf_freq) are assumed to be sorted in their id's (this has to be done beforehand). Active_cntr_siz is the total number of counters being used in the counter array by some ids (i.e., number of distinct elements whose approximate counts are remembered by the algorithm at this stage). bf_siz is the total number of distinct elements (ids) in the incoming packet. A temporary counter array (temp_cntr_id, temp_cntr_freq) of size active_cntr_siz + bf_siz (max possible distinct elements after merging) will be used to merge the two id arrays. If same id's exist in both the arrays, their counts will be added. 

If the size of (temp_cntr_id,temp_cntr_freq) is higher than cntr_siz (the number of counters available to the algorithm), some elements will be dropped (the ids with low counts) till the size becomes less than or equal to cntr_siz. The contents will be copied to the (cntr_id,cntr_freq) once this criterion is met.

Algorithm: FREQUENT algorithm (Cormode's paper: Finding Frequent Items in Data Streams). A modified implementation is used to promote parallel processing.

INPUTS: 

cntr_id: Array containing the id data currently stored by the counters.

cntr_freq: Array containing the corresponding frequency data.

cntr_siz: Total number of counters ( size of (cntr_id,cntr_freq) ) available to do the counting.

ptr_active_cntr_siz: Number of counters in the (cntr_id, cntr_freq) counter array which are currently being used by some ids (active_cntr_siz).

bf_id: Array containing the incoming id data

bf_freq: Array containing the corresponding frequency data

bf_siz: Size of the new input (bf_id, bf_freq) data

 */
//--------------------------------------------------------------------------
{

  int status = 0;

  int * temp_cntr_id = NULL;
  int * temp_cntr_freq = NULL; 

  /* check inputs */

  if ( cntr_id == NULL ) { go_BYE(-1); }
  if ( cntr_freq == NULL ) { go_BYE(-1); }
  if ( ptr_active_cntr_siz == NULL ) { go_BYE(-1); }
  if ( bf_id == NULL ) { go_BYE(-1); }
  if ( bf_freq == NULL ) { go_BYE(-1); }

  //------------------------------------------------------------------------

  /* (temp_cntr_id,temp_cntr_freq) stores the merged and sorted (sorted in id) data of the counters (cntr_id,cntr_freq) and (bf_id, bf_freq) */

  long long ii = 0, jj = 0, kk = 0; 

  temp_cntr_id = (int *)malloc( ((*ptr_active_cntr_siz)+bf_siz)*sizeof(int) );
  temp_cntr_freq = (int *)malloc( ((*ptr_active_cntr_siz)+bf_siz)*sizeof(int) );

  
  while (1) {

    if ( ii < (*ptr_active_cntr_siz) && jj < bf_siz ) {
      
      if ( cntr_id[ii] < bf_id[jj] ) {
	temp_cntr_id[kk] = cntr_id[ii];
	temp_cntr_freq[kk++] = cntr_freq[ii++];
      }
      else if ( bf_id[jj] < cntr_id[ii] ) {
	temp_cntr_id[kk] = bf_id[jj];
	temp_cntr_freq[kk++] = bf_freq[jj++];
      }
      else {
	temp_cntr_id[kk] = bf_id[jj];
	temp_cntr_freq[kk++] = bf_freq[jj++] + cntr_freq[ii++];
      }

    }
    else if ( ii < (*ptr_active_cntr_siz) && jj == bf_siz ) {
      temp_cntr_id[kk] = cntr_id[ii];
      temp_cntr_freq[kk++] = cntr_freq[ii++];
    }
    else if ( ii == (*ptr_active_cntr_siz) && jj < bf_siz ) {
      temp_cntr_id[kk] = bf_id[jj];
      temp_cntr_freq[kk++] = bf_freq[jj++];
    }
    else {
      break;
    }
  }
  
  *ptr_active_cntr_siz = kk;

  //------------------------------------------------------------------------
  
  /* If the size of (temp_cntr_id,temp_cntr_freq) is less than cntr_siz (i.e., teh total number of counters available for use) then we just copy the data to (cntr_id, cntr_freq) (overwriting). Else, keep dropping elements with low frequencies (according to FREQUENT algorithm's rules so that theoretical guarantees hold)till the size of (temp_cntr_id,temp_cntr_freq) becomes less than cntr_siz and then copy the data to (cntr_id, cntr_freq). */


  while ( *ptr_active_cntr_siz > cntr_siz ) { 
     
    for ( long long kk = 0; kk < *ptr_active_cntr_siz; kk++ ) {
      temp_cntr_freq[kk]--;
    }

    jj = 0;
    for ( long long kk = 0; kk < *ptr_active_cntr_siz; kk++ ) {
      if ( temp_cntr_freq[kk] > 0 ) {
	temp_cntr_freq[jj] = temp_cntr_freq[kk];
	temp_cntr_id[jj++] = temp_cntr_id[kk];
      }
    } 
    *ptr_active_cntr_siz = jj;

  }

  memcpy(cntr_id, temp_cntr_id, *ptr_active_cntr_siz*sizeof(int));
  memcpy(cntr_freq, temp_cntr_freq, *ptr_active_cntr_siz*sizeof(int));

 BYE:
  free_if_non_null(temp_cntr_id);
  free_if_non_null(temp_cntr_freq);

  return(status);

}
