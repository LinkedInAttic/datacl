#include <stdbool.h>
#include "qtypes.h"
#include "bin_search_I4.h"
#ifdef CILK
#include <cilk/cilk.h>
#endif

// START FUNC DECL
int 
core_is_a_in_b_I4(
			 int nT,
			 long long block_size,
			 char *f1,
			 char *nn_f1,
			 long long nR1,
			 int minval,
			 int maxval,
			 char *cfld,
			 char *f2,
			 long long nR2
			 )
// STOP FUNC DECL
{
  int status = 0;
  /* TODO P3: Keep a cache of known values so that we do not have to
   * look all over T2 */

  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR1; }

    // These are kept local to avoid sharing issues
    register int *l_f1     = (int *)f1;
    register int *l_f2     = (int *)f2;
    register char *l_cfld      = cfld; // make a local copy
    int f1_val;
    //-----------------------------------
    for  ( long long i = lb; i < ub; i++ ) {
      /* If f1 is null, then cfld and dst_f1 must also be null */
      if ( ( nn_f1 != NULL ) && ( nn_f1[i] == FALSE ) ) {
	l_cfld[i] = FALSE; /* IMP: cfld is not undefined but false */
	continue;
      }
      /*----------------------------------------------------------------*/
      f1_val = l_f1[i];
      if ( ( f1_val < minval ) || ( f1_val > maxval ) ) {
	l_cfld[i] = FALSE; /* IMP: cfld is not undefined but false */
      }
      /*----------------------------------------------------------------*/
      if ( nR2 < 16 ) {  /* small means do sequential search */
	int sum = 0;
	for ( int k = 0; k < nR2; k++ ) {  /* vectorizes nicely */
	  sum += (l_f2[k] == f1_val );
	}
	if ( sum > 0 ) { l_cfld[i] = 1; } else { l_cfld[i] = 0; }
      }
      else {
        long long pos;
	status = bin_search_I4(l_f2, nR2, f1_val, &pos, "");
	if ( status < 0 ) { continue; }
        if ( pos >= 0 ) { l_cfld[i] = 1; } else { l_cfld[i] = 0; }
      }
      /*-------------------------------------------------------------*/
    }
  }
  cBYE(status);
 BYE:
  return status ;
}
