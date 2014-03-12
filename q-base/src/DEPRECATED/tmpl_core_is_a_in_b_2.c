#include <stdbool.h>
#include "qtypes.h"
#include "bin_search___XTYPE__.h"
#ifdef CILK
#include <cilk/cilk.h>
#endif

// START FUNC DECL
int 
core_is_a_in_b___XTYPE__(
			 int nT,
			 long long block_size,
			 char *f1,
			 char *nn_f1,
			 long long nR1,
			 __ITYPE__ minval,
			 __ITYPE__ maxval,
			 char *cfld,
			 char *dst_f1,
			 char *nn_dst_f1,
			 char *src_f2,
			 long long nR2,
			 bool write_dst
			 )
// STOP FUNC DECL
{
  int status = 0;
  /* TODO P3: Keep a cache of known values so that we do not have to
   * look all over T2 */

  cilkfor ( int tid = 0; tid < nT; tid++ ) {
    long long lb = 0 + (tid * block_size);
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR1; }

    // These are kept local to avoid sharing issues
    register __ITYPE__ *l_f1     = (__ITYPE__ *)f1;
    register __ITYPE__ *l_src_f2 = (__ITYPE__ *)src_f2;
    register __ITYPE__ *l_dst_f1 = (__ITYPE__ *)dst_f1;
    register char *l_cfld      = cfld; // make a local copy
    register char *l_nn_dst_f1 = nn_dst_f1; // make a local copy

    int f1_val;
    //-----------------------------------
    for  ( long long i = lb; i < ub; i++ ) {
      long long pos;
      /* If f1 is null, then cfld and dst_f1 must also be null */
      if ( ( nn_f1 != NULL ) && ( nn_f1[i] == FALSE ) ) {
	if ( write_dst ) { 
	  l_dst_f1[i] = 0; l_nn_dst_f1[i] = 0; 
	}
	else {
	  l_cfld[i] = FALSE; /* IMP: cfld is not undefined but false */
	}
	continue;
      }
      /*----------------------------------------------------------------*/
      f1_val = l_f1[i];
      if ( ( f1_val < minval ) || ( f1_val > maxval ) ) {
	pos = -1;
	if ( write_dst ) { 
	  l_dst_f1[i] = 0; l_nn_dst_f1[i] = 0; 
	}
	else {
	  l_cfld[i] = FALSE; /* IMP: cfld is not undefined but false */
	}
	continue;
      }
      /*----------------------------------------------------------------*/
      if ( nR2 < 16 ) {  /* small means do sequential search */
	pos = -1;
	for ( int k = 0; k < nR2; k++ ) { 
	  if ( l_src_f2[k] == f1_val ) { 
	    pos = k; break; 
	  }
	}
      }
      else {
	status = bin_search___XTYPE__(l_src_f2, nR2, f1_val, &pos, "");
      }
      //---------------------------------------------
      if ( pos >= 0 ) { 
	if ( write_dst ) { 
	  l_dst_f1[i] = l_src_f2[pos];
	  l_nn_dst_f1[i] = 1;
	}
	else {
	  l_cfld[i] = 1;
	}
      }
      else {
	if ( write_dst ) { 
	  l_dst_f1[i] = 0;
	  l_nn_dst_f1[i] = 0;
	}
	else {
	  l_cfld[i] = 0;
	}
      }
      //---------------------------------------------
    }
  }
  cBYE(status);
 BYE:
  return(status);
}
