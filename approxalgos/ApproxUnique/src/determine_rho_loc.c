#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include "constants.h"
#include "macros.h"
#include "determine_rho_loc.h"

// START FUNC DECL
int 
determine_rho_loc (
		   uint64_t hash_val,
		   int m,
		   int *ptr_rho,
		   int *ptr_loc
		   )
// STOP FUNC DECL
//---------------------------------------------------------------------------
/* README: 

determine_rho_loc(hash_val,m,ptr_rho,ptr_loc): This function calculates the rho and loc value for a given hash value and number of bins. rho is the location of the least significant 1 in the binary representation of hash_val. loc is the bin id to which the input element is assigned. 

INPUTS:

hash_val: 64-bit integer produced by some hash function.

m: Number of bins used by the algorithm.

OUTPUTS: 

ptr_rho: Location where the rho value is stored. ex: if hash_val = 0b1001111000100 then rho = 3, if hash_val = 0b010101111 then rho = 1.

ptr_loc: Location where the loc value is stored. If m bins are used, this value is going to be in between 0 and m-1.

 */
//---------------------------------------------------------------------------
{
  
  int status = 0;
 
  if ( hash_val == 0 ) { go_BYE(-1); }

  *ptr_rho = -1; 
  *ptr_loc = -1; 
  
  /* Last (64-20) bits of hash_val's binary representation are used to calculate rho */
     
  int rho = __builtin_ctzll(hash_val)+1;

  /* First 20 bits of hashval's binary representation are used to calculate loc. */

  unsigned int uint_hash_val = (unsigned int) (hash_val >> (64-20));
  int loc = (int) (uint_hash_val & (m-1));

  /* Sanity checking: A rho value of greater than (64 - 20) would imply one of the two things:
(1) The odd chance that hash_val is divisible by 2^44. Just retrying might help unless the cardinality is higher than 1 trillion. 
(2) more hash bits are needed to calculate rho. Use 128 bit hashfunctions (like spooky_hash128) and change this code accordingly in that case (this won't occur unless you are dealing with data sets with greater than 100 billion unique elements, which rarely happens). 

  */

  if ( rho > (64-20) ) { go_BYE(-1); }

  *ptr_rho = rho;
  *ptr_loc = loc;
 
 BYE:
  return (status);
}

