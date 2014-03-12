#include <values.h>
#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "par_f_to_s.h"
#include "./AUTOGEN/GENFILES/extract_I2.h"
#include "./AUTOGEN/GENFILES/extract_I4.h"
#include "./AUTOGEN/GENFILES/extract_I8.h"

#include "incl_f_to_s_0_min.c"
#include "incl_f_to_s_0_max.c"
#include "incl_f_to_s_0_sum.c"
// START FUNC DECL
int
par_f_to_s(
	   char *X,
	   FLD_TYPE fldtype,
	   char *nn_X,
	   long long lb,
	   long long ub,
	   const char *op,
	   char *rslt_buf,
	   int sz_rslt_buf
	   )
// STOP FUNC DECL
{
  int status = 0;
  long long ll_minval = LLONG_MAX;
  double    dd_minval = DBL_MAX;
  long long ll_maxval = LLONG_MIN;
  double    dd_maxval = DBL_MIN;
  long long ll_sum = 0;
  double    dd_sum = 0.0;
  long long cum_nn_cnt = 0;

  char      i1val[MAX_NUM_THREADS];
  short     i2val[MAX_NUM_THREADS];
  int       i4val[MAX_NUM_THREADS];
  long long i8val[MAX_NUM_THREADS];
  float     f4val[MAX_NUM_THREADS];
  double    f8val[MAX_NUM_THREADS];
  long long nn_cnt[MAX_NUM_THREADS];
  long long ll_numer[MAX_NUM_THREADS];
  double    dd_numer[MAX_NUM_THREADS];
  long long tlb[MAX_NUM_THREADS];
  long long tub[MAX_NUM_THREADS];
  int nT; 
  long long nR;

  // Break up along nice boundaries
  if ( ( ( lb / 1024 ) * 1024 ) != lb )  { go_BYE(-1); }
  status = partition(lb, ub, &nR, &nT, tlb, tub); cBYE(status);

  for ( int i = 0; i < nT; i++ ) { 
    nn_cnt[i]    = 0; 
    ll_numer[i]  = 0; 
    dd_numer[i]  = 0; 
  }
  //----------------------------------------
  if ( strcmp(op, "min") == 0 ) { 
#include "incl_f_to_s_1_min.c"
  }
  else if ( strcmp(op, "max") == 0 ) { 
#include "incl_f_to_s_1_max.c"
  }
  else if ( strcmp(op, "sum") == 0 ) { 
#include "incl_f_to_s_1_sum.c"
  }
  else { go_BYE(-1); }
  // Now combine results 
  if ( strcmp(op, "min") == 0 ) { 
#include "incl_f_to_s_2_min.c"
  }
  else if ( strcmp(op, "max") == 0 ) { 
#include "incl_f_to_s_2_max.c"
  }
  else if ( strcmp(op, "sum") == 0 ) {
#include "incl_f_to_s_2_sum.c"
  }
  else {
    go_BYE(-1);
  }
 BYE:
  return(status);
}
