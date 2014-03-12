#include "constants.h"
#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

extern int g_num_cores;
// START FUNC DECL
int
xfer_SC_I4(
	   char *src_fld_X, 
	   int fld_len,
	   long long src_nR,
	   char *dst_idx_X, 
	   char *nn_dst_idx_X, 
	   long long dst_nR,
	   char *dst_fld_X
	   )
// STOP FUNC DECL
{
  int status = 0;

  int nT = g_num_cores;
  long long block_size = dst_nR / nT;
  int  *idxptr = (int  *)dst_idx_X;
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = dst_nR; }

    for ( long long i = lb; i < ub; i++ ) {
      char *dst_ptr = dst_fld_X + (i   * (fld_len+1) );
      if ( ( nn_dst_idx_X == NULL ) || ( nn_dst_idx_X[i] == 0 ) ) {
	/* write null characters */
#pragma omp simd
	for ( int j = 0; j < fld_len+1; j++ ) { 
	  dst_ptr[j] = '\0';
	}
      }
      else {
	int idx = idxptr[i];
	if ( ( idx < 0 ) || ( idx >= src_nR ) ) { status = -1; continue; }
	char *src_ptr = src_fld_X  + (idx * (fld_len+1) );
	memcpy(dst_ptr, src_ptr, (fld_len+1));
      }
    }
  }
  return status;
}
