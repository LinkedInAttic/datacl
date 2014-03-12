#include "constants.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include "assign_I8.h"
// START FUNC DECL
int
par_countf___XTYPE__(
		  int *src_fld,  /* [src_nR] */
		  __ITYPE__ *fk_fld,  /* [src_nR] */
		  long long src_nR,
		  long long * restrict cntI8, /* [dst_nR] */
		  int dst_nR,
		  int nT
		  )
// STOP FUNC DECL
{
  int status = 0;
  long long **par_cnt = NULL;
  par_cnt = malloc(nT * sizeof(long long *));
  return_if_malloc_failed(par_cnt);
  for ( int tid = 0; tid < nT; tid++ ) { par_cnt[tid] = NULL; }
  int t_block_size = src_nR / nT;
  cilkfor ( int tid = 0; tid < nT; tid++ ) { 
    par_cnt[tid] = malloc(dst_nR * sizeof(long long));
    assign_const_I8(par_cnt[tid], dst_nR, 0);
    long long t_lb = tid  * t_block_size;
    long long t_ub = t_lb + t_block_size;
    if ( tid == (nT-1) ) { t_ub = src_nR; }
    for ( long long jj = t_lb; jj < t_ub; jj++ ) { 
	int idx = fk_fld[jj];
	int val  = src_fld[jj]; 
	par_cnt[tid][idx] += val;
    }
  }
  for ( int ii = 0; ii < dst_nR; ii++ ) { 
    long long lsum = 0;
    for ( int tid = 0; tid < nT; tid++ ) { 
      lsum += par_cnt[tid][ii];
    }
    cntI8[ii] += lsum;
  }
  for ( int tid = 0; tid < nT; tid++ ) { 
    free(par_cnt[tid]); par_cnt[tid] = NULL;
  }
  free(par_cnt); par_cnt = NULL;
 BYE:
  return(status);
}
