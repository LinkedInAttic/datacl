#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mmap.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "f1opf2_cum.h"

#include "f_to_s_sum_I1.h"
#include "f_to_s_sum_I4.h"
#include "f_to_s_sum_I8.h"

#include "cum_I4_to_I4.h"
#include "cum_I4_to_I8.h"
#include "cum_I8_to_I4.h"
#include "cum_I8_to_I8.h"
#include "cum_I1_to_I4.h"
#include "cum_I1_to_I8.h"

// START FUNC DECL
int 
par_f1opf2_cum(
	       const char *in_X,
	       long long nR,
	       FLD_TYPE f1type, /* input type */
	       FLD_TYPE f2type, /* output type */
	       const char *out_X
	       )
// STOP FUNC DECL
{
  int status = 0;
  char       *f1I1 = NULL;
  int        *f1I4 = NULL, *f2I4 = NULL;
  long long  *f1I8 = NULL, *f2I8 = NULL;
#define CUM_NUM_THREADS 256
  long long partial_sums[CUM_NUM_THREADS];

  if ( nR < 1048576 ) { go_BYE(-1); }
  for ( int i = 0; i < CUM_NUM_THREADS; i++ ) { 
    partial_sums[i] = 0;
  }
  //--------------------------------------------
  // Compute sum for each block 
  int nT = CUM_NUM_THREADS;
  long long block_size = nR / nT;
  cilkfor ( int tid = 0; tid < CUM_NUM_THREADS; tid++ ) { 
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    long long nX = ub - lb; 
    if ( tid == (nT-1) ) { ub = nR; }
    switch ( f1type ) {
    case I4 : 
      f1I4 = (int *)in_X;
      f_to_s_sum_I4(f1I4, nX, &(partial_sums[tid]));
      break;
    case I8 : 
      f1I8 = (long long *)in_X;
      f_to_s_sum_I8(f1I8, nX, &(partial_sums[tid]));
      break;
    case I1 : 
      f1I1 = (char *)in_X;
      f_to_s_sum_I1(f1I1, nX, &(partial_sums[tid]));
      break;
    default : 
      status = -1; 
      break;
    }
  }
  cBYE(status);
  // Now compute cum for each block using sum of previous block as offset

  cilkfor ( int tid = 0; tid < CUM_NUM_THREADS; tid++ ) { 
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    long long nX = ub - lb; 
    if ( tid == (nT-1) ) { ub = nR; }
    if ( t == 0 ) { 
      offI4 = offI8 = 0;
    }
    else {
      switch ( f2type ) { 
	case I4 : offI4 = (int)partial_sums[tid-1]; break;
	case I8 : offI8 =      partial_sums[tid-1]; break;
	default : status = -1; break;
      }

    switch ( f1type ) {
    case I4 : 
      f1I4 = (int *)in_X; f1I4 += lb;
      switch ( f2type ) { 
      case I4: 
        f2I4 = (int *)out_X; f2I4 += lb;
        cum_I4_to_I4(f1I4, nX, f2I4);
	incr_I4(f2I4, 
	break;
      case I8: 
        f2I8 = (long long *)out_X; f2I8 += lb;
	cum_I4_to_I8(f1I4, nX, f2I8, offI8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
      break;
    case I8 : 
      f1I8 = (long long *)in_X; f1I8 += lb;
      switch ( f2type ) { 
      case I4: 
        f2I4 = (int *)out_X; f2I4 += lb;
	cum_I8_to_I4(f1I8, nX, f2I4, offI4);
      case I8: 
        f2I8 = (long long *)out_X; f2I8 += lb;
	cum_I8_to_I8(f1I8, nX, f2I8, offI8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
      break;
    case I1 : 
      f1I1 = (char *)in_X; f1I1 += lb;
      switch ( f2type ) { 
      case I4: 
        f2I4 = (int *)out_X; f2I4 += lb;
	cum_I1_to_I4(f1I1, nX, f2I4, offI4);
	break;
      case I8: 
        f2I8 = (long long *)out_X; f2I8 += lb;
	cum_I1_to_I8(f1I1, nX, f2I8, offI8);
	break;
      default : 
	go_BYE(-1);
	break;
      }
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
 BYE:
  return(status);
}
