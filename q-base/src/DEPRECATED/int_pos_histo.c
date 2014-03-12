#include "ipps.h"
#include "qtypes.h"
#include "mmap.h"
#include "int_pos_count.h"
#include "meta_globals.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "aux_meta.h"
#include "mk_file.h"
#include "assign_int.h"

#include "pos_count_I4_I4.h"
#include "pos_count_I4_I8.h"
#include "pos_count_I8_I4.h"
#include "pos_count_I8_I8.h"

/* START FUNC DECL */
int 
int_pos_count(
	      FLD_TYPE f1type,
	      char *f1_X,
	      long long nR1,
	      FLD_TYPE f2type,
	      char *f2_X,
	      long long nR2,
	      char *op_X, 
	      long long nR3
	      )
/* START FUNC DECL */
{
  int status = 0;
  int *I4f3 = (int *)op_X;
  long long ltemp, tlb[MAX_NUM_THREADS], tub[MAX_NUM_THREADS]; int nT;
  /*-------------------------------------------------------- */
  /* Set counters to 0  */
  status = partition(0, nR3, &ltemp, &nT, tlb, tub); cBYE(status);
  cilkfor ( int tid = 0; tid < nT; tid++ ) { 
    long long lb = tlb[tid];
    long long ub = tub[tid];
    int *iptr = I4f3; iptr += lb;
    // assign_const_int(iptr, 0, (ub-lb));
    ippsSet_32s(0, iptr, (ub-lb));
  }

  /* If nR3 is small enough, then the cost of duplicating it is not too  high */
  int *I4f1 = (int *)f1_X;
  long long *I8f1 = (long long *)f1_X;
  int *I4f2 = (int *)f2_X;
  long long *I8f2 = (long long *)f2_X;
  switch ( f1type ) {
  case I4 : 
    switch ( f2type ) { 
    case I4 : 
      status = pos_count_I4_I4(I4f1, nR1, I4f2, nR2, I4f3, nR3);
      break;
    case I8 : 
      status = pos_count_I4_I8(I4f1, nR1, I8f2, nR2, I4f3, nR3);
      break;
    case I1 : case I2 : case F4: case F8: default : 
      go_BYE(-1);
      break;
    }
    break;
  case I8 : 
    switch ( f2type ) { 
    case I4 : 
      status = pos_count_I8_I4(I8f1, nR1, I4f2, nR2, I4f3, nR3);
      break;
    case I8 : 
      status = pos_count_I8_I8(I8f1, nR1, I8f2, nR2, I4f3, nR3);
      break;
    case I1 : case I2 : case F4: case F8: default : 
      go_BYE(-1);
      break;
    }
    break;
  case I1 : case I2 : case F4: case F8: default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return(status);
}
