#include "../../constants.h"
#include "../../macros.h"
#include <stdio.h>
#include <stdbool.h>

extern int g_num_cores;
// START FUNC DECL
int
xfer_I1_I4(
				  char *src_fld_X, 
				  char *nn_src_fld_X, 
				  long long src_nR,
				  char *dst_idx_X, 
				  char *nn_dst_idx_X, 
				  long long dst_nR,
				  char *dst_fld_X, 
				  char *nn_dst_fld_X
				  )
// STOP FUNC DECL
{
  int status = 0;

#pragma cilk grainsize = 4096
  int nT = g_num_cores;
  long long block_size = dst_nR / nT;
#pragma omp parallel
#pragma omp for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = dst_nR; }

  for ( long long i = lb; i < ub; i++ ) {
    char  val;
    int   idx;

    int  *idxptr = (int  *) dst_idx_X;
    char *valptr = (char *)src_fld_X;
    char *outptr = (char *)dst_fld_X;

    if ( nn_dst_fld_X != NULL ) { 
      nn_dst_fld_X[i] = 0; /* default assumption */
    }
    outptr[i]       = 0;
    if ( ( nn_dst_idx_X != NULL ) && ( nn_dst_idx_X[i] == 0 ) ) {
      /* nothing to do */
    }
    else {
      idx = idxptr[i];
      if ( ( idx < 0 ) || ( idx >= src_nR ) ) { status = -1; continue; }
      val = valptr[idx];
      if ( ( nn_src_fld_X != NULL ) && ( nn_src_fld_X[idx] == 0 ) ) {
        /* nothing to do */
      }
      else {
	if ( nn_dst_fld_X != NULL ) { 
          nn_dst_fld_X[i] = 1;
	}
        outptr[i]       = val;
      }
    }
  }
  }
return status;
}
