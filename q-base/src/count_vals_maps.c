#include <stdio.h>
#include "count_vals_maps.h"
#include "count_vals_maps_cc.h"
#include "macros.h"

// START FUNC DECL
int count_vals_maps (
		     int * x,
		     char * cfld,
		     long long siz,
		     FILE * idfp,
		     FILE * freqfp,
		     long long * out_siz
		     )
// STOP FUNC DECL
//-----------------------------------------------------------------------
/* README: 

count_vals_maps: takes as input an array of integers and outputs them in the form of (id, freq), i.e, which are the elements in the set and how many times they occur. The set of id's will be written in the binary format to the file pointed by idfp and the set with their corresponding frequencies will be written in the binary format to the file pointed by freqfp. The sets will be sorted in id.

NOTE: std::maps of C++ will be used to do the calculations (red-black trees)

For ex: take the input set {1,2,3,1,2,2,2,2,1,1}. ids are {1,2,3} (sorted) and their corresponding frequencies are {4,5,1}. 


INPUTS: 

x: the array containing the input elements 

cfld: character array acting as select operator for x. Two modes:
(i) NULL: all elements of x will be processed
(i) non-NULL: only those elements with non-zero cfld entries will be processed.

siz: number of elements in the input set

OUTPUTS:

idfp: binary file pointer, the ids in the input set will be written here in sorted order

freqfp: binary file pointer, the corresponding frequencies of the ids will be written here

out_siz: number of elements in the output binary file ( same as the number of distinct elements in the input set)

*/
//-----------------------------------------------------------------------
{

  int status; 

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( idfp == NULL ) { go_BYE(-1); }
  if ( freqfp == NULL ) { go_BYE(-1); }
  if ( siz <= 0 ) { go_BYE(-1); }

  status = count_vals_maps_cc (x, cfld, siz, idfp, freqfp, out_siz);
  cBYE(status);

 BYE:
  return (status);
}
