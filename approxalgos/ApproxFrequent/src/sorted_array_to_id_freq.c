#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "sorted_array_to_id_freq.h"
#include "macros.h"

// START FUNC DECL
int
sorted_array_to_id_freq (
		   int * buf,          
		   long long num_buf,  
		   int * bf_id,        
		   int * bf_freq,      
		   long long * ptr_bf_siz 
		   )
// STOP FUNC DECL
//--------------------------------------------------------------------------
/* README:

sorted_array_to_id_freq(buf,num_buf,bf_id,bf_freq,ptr_bf_siz): This function takes a sorted array as input and converts it into (id, freq) format. For example: if the input was {1,1,1,2,4,4} the ids  would be {1,2,4} and their corresponding frequencies would be {3,1,2} as 1 occurs 3 times, 2 occurs once and 4 occurs 2 times.

NOTE: you have to ensure beforehand that the input is sorted.

INPUTS:

buf: Array containing the input sorted data.

num_buf: Number of elements in the input.

OUTPUTS:

bf_id: Array containing the ids in the input data.

bf_freq: Array containing the frequencies corresponding to the keys in the input data.

ptr_bf_siz: Pointer to a location containing the number of ids in the input data

 */
//--------------------------------------------------------------------------
{

  int status = 0;

  /* check inputs */

  if ( buf == NULL ) { go_BYE(-1); }
  if ( bf_id == NULL ) { go_BYE(-1); }
  if ( bf_freq == NULL ) { go_BYE(-1); }
  if ( ptr_bf_siz == NULL ) { go_BYE(-1); }

  /* (id, freq) conversion of sorted data */

  long long ii = 0, jj = 0;
  int temp_freq = 1;

  while ( ii < num_buf-1 ) {
    
    if ( buf[ii] == buf[ii+1] ) {
      temp_freq++;
    }
    else {
      bf_id[jj] = buf[ii];
      bf_freq[jj++] = temp_freq;
      temp_freq = 1;
    }  
    ii++;
  }
  bf_id[jj] = buf[ii];
  bf_freq[jj] = temp_freq;
  jj++;

  *ptr_bf_siz = jj; 


 BYE:
  return(status);

}
