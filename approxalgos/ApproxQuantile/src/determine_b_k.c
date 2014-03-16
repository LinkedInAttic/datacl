#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>
#include "constants.h"
#include "macros.h"
#include "determine_b_k.h"
#include <math.h>
#include "approx_quantile.h"

// START FUNC DECL
int
determine_b_k(
	      double eps,  
	      long long siz, 
	      int *ptr_b,  
	      long long *ptr_k  
	      )
// STOP FUNC DECL
//---------------------------------------------------------------------------
/* README:

determine_b_k (err,siz,ptr_b,ptr_k): This function calculates b and k values for a given data size and acceptable error percentage. b is the number of buffers and k is the size of each buffer in the 2d buffer array.

INPUTS: 

err: Acceptable error percentage in the quantile calculations. ex: 0.001 implies +/- 0.1% and so on.

siz: Size of the input data

OUTPUTS:

ptr_b: Pointer to the location which tells you how many buffers to use in the 2d buffer array (b)

ptr_k: Pointer to the location which tells you the size of each buffer in the 2d buffer array (k)

*/

//---------------------------------------------------------------------------
{

  int status = 0;

  int b = -1;
  long long k = -1;

  b = 2; 
  while ( ((b-2)*pow(2,b-2)+1) < (double)(eps*siz) ) {
    b++;
  }
  b--;

  if ( (siz % (long long)pow(2,b-1)) == 0 ) {
    k = siz/pow(2,b-1);
  }
  else {
    k = siz/pow(2,b-1)+1;
  }

  if ( k < 10000 ) { k = 10000; } /* to give some freedom for cilkfor */

  *ptr_b = b;
  *ptr_k = k;

  return(status);
}
