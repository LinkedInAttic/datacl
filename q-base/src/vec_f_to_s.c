#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "vec_f_to_s.h"

#include "incl_f_to_s_min.c"
#include "incl_f_to_s_max.c"
#include "incl_f_to_s_sum.c"

#include "f_to_s_sum_B.h"

extern int g_num_cores;
// START FUNC DECL
int
vec_f_to_s(
	   char *X,
	   FLD_TYPE fldtype,
	   char *nn_X,
	   long long nR,
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

  char *part_rslt = NULL;

  long long *nn_cnt = NULL;
  long long *ll_numer = NULL;
  double    *dd_numer = NULL;
  int nT; long long block_size;

  // Break up along nice boundaries
  int max_nT = g_num_cores;
  status = partition(nR, 1024, max_nT, &block_size, &nT);
  cBYE(status);
  nn_cnt = malloc(nT * sizeof(long long)); return_if_malloc_failed(nn_cnt);
  ll_numer = malloc(nT * sizeof(long long)); return_if_malloc_failed(ll_numer);
  dd_numer = malloc(nT * sizeof(double)); return_if_malloc_failed(dd_numer);
  /* 16 is the largest size of a basic type. double = 8 bytes */
  part_rslt = malloc(nT * 16); return_if_malloc_failed(part_rslt);

  char *I1_part_rslt = (char *)part_rslt;
  short *I2_part_rslt = (short *)part_rslt;
  int *I4_part_rslt = (int *)part_rslt;
  long long *I8_part_rslt = (long long *)part_rslt;
  float *F4_part_rslt = (float *)part_rslt;
  double *F8_part_rslt = (double *)part_rslt;

#ifdef IPP
  ippsSet_64s((long long)0, nn_cnt,   nT);
  ippsSet_64s((long long)0, ll_numer, nT);
  ippsSet_64f((double)0,    dd_numer, nT);
#else
  for ( int i = 0; i < nT; i++ ) { 
    nn_cnt[i]    = 0; 
    ll_numer[i]  = 0; 
    dd_numer[i]  = 0; 
  }
#endif
  //----------------------------------------
#pragma omp parallel for
  for ( int tid = 0; tid < nT; tid++ ) { // POTENTIAL CILK LOOP
    if ( status < 0 ) { continue; }
    /* We store results for each iteration of above loop in a "local"
     * variable" and assign it to the partial results at end */
    long long lb = tid * block_size;
    long long ub = lb + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    long long nX = (ub -lb);
    if ( nX <= 0 ) { status = -1; continue; }
    if ( strcmp(op, "min") == 0 ) {
      char      i1val = SCHAR_MAX, *I1_X = NULL;
      short     i2val = SHRT_MAX,  *I2_X = NULL;
      int       i4val = INT_MAX,   *I4_X = NULL;
      long long i8val = LLONG_MAX, *I8_X = NULL;
      float     f4val = FLT_MAX,   *F4_X = NULL;
      double    f8val = DBL_MAX,   *F8_X = NULL;

      if ( nn_X == NULL ) { 
	nn_cnt[tid] = nX; // all values are defined 
	switch ( fldtype ) {
	case I1 : 
	  I1_X = (char *)X; I1_X += lb;
	  f_to_s_min_I1(I1_X, nX, &i1val);
	  I1_part_rslt[tid] = i1val;
	  break;
	case I2 : 
	  I2_X = (short *)X; I2_X += lb;
#ifdef IPP
	  if ( nX < INT_MAX ) { 
	    ippsMin_16s(I2_X, nX, &i2val);
	  }
	  else {
	    f_to_s_min_I2(I2_X, nX, &i2val);
	  }
#else
	  f_to_s_min_I2(I2_X, nX, &i2val);
#endif
	  I2_part_rslt[tid] = i2val;
	  break;
	case I4 : 
	  I4_X = (int *)X; I4_X += lb;
#ifdef IPP
	  if ( nX < INT_MAX ) { 
	    ippsMin_32s(I4_X, nX, &i4val);
	  }
	  else {
	    f_to_s_min_I4(I4_X, nX, &i4val); 
	  }
#else
	  f_to_s_min_I4(I4_X, nX, &i4val); 
#endif
	  I4_part_rslt[tid] = i4val;
	  break;
	case I8 : 
	  I8_X = (long long *)X; I8_X += lb;
	  f_to_s_min_I8(I8_X, nX, &i8val); 
	  I8_part_rslt[tid] = i8val;
	  break;
	case F4 : 
	  F4_X = (float *)X; F4_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMin_32f(F4_X, nX, &f4val);
	  }
	  else {
	    f_to_s_min_F4(F4_X, nX, &f4val);
	  }
#else
	  f_to_s_min_F4(F4_X, nX, &f4val);
#endif
	  F4_part_rslt[tid] = f4val;
	  break;
	case F8 : 
	  F8_X = (double *)X; F8_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMin_64f(F8_X, nX, &f8val);
	  }
	  else {
	    f_to_s_min_F8(F8_X, nX, &f8val);
	  }
#else
	  f_to_s_min_F8(F8_X, nX, &f8val);
#endif
	  F8_part_rslt[tid] = f8val;
	  break;
	default : 
	  status = -1; 
	  break;
	}
      }
      else {
	nn_cnt[tid] = 0; // no idea how many values are defined 
	long long l_nn_cnt = 0;
	switch ( fldtype ) {
	case I1 : 
	  nn_f_to_s_min_I1(((char *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i1val);
	  I1_part_rslt[tid] = i1val;
	  break;
	case I2 : 
	  nn_f_to_s_min_I2(((short *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i2val); 
	  I2_part_rslt[tid] = i2val;
	  break;
	case I4 : 
	  nn_f_to_s_min_I4(((int *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i4val);
	  I4_part_rslt[tid] = i4val;
	  break;
	case I8 : 
	  nn_f_to_s_min_I8(((long long *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i8val); 
	  I8_part_rslt[tid] = i8val;
	  break;
	case F4 : 
	  nn_f_to_s_min_F4(((float *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &f4val);
	  F4_part_rslt[tid] = f4val;
	  break;
	case F8 : 
	  nn_f_to_s_min_F8(((double *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &f8val);
	  F8_part_rslt[tid] = f8val;
	  break;
	default : 
	  status = -1; 
	  break;
	}
        nn_cnt[tid] = l_nn_cnt;
      }
    }
    else if ( strcmp(op, "max") == 0 ) {
      char      i1val = SCHAR_MIN, *I1_X = NULL;
      short     i2val = SHRT_MIN,  *I2_X = NULL;
      int       i4val = INT_MIN,   *I4_X = NULL;
      long long i8val = LLONG_MIN, *I8_X = NULL;
      float     f4val = FLT_MIN,   *F4_X = NULL;
      double    f8val = DBL_MIN,   *F8_X = NULL;

      if ( nn_X == NULL ) {
	nn_cnt[tid] = nX; // all values are defined 
	switch ( fldtype ) {
	case I1 : 
	  I1_X = (char *)X; I1_X += lb;
	  f_to_s_max_I1(I1_X, nX, &i1val); 
	  I1_part_rslt[tid] = i1val;
	  break;
	case I2 : 
	  I2_X = (short *)X; I2_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMax_16s(I2_X, nX, &i2val);
	  }
	  else {
	    f_to_s_max_I2(I2_X, nX, &i2val);
	  }
#else
	  f_to_s_max_I2(I2_X, nX, &i2val);
#endif
	  I2_part_rslt[tid] = i2val;
	  break;
	case I4 : 
	  I4_X = (int *)X; I4_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMax_32s(I4_X, (ub -lb), &i4val);
	  }
	  else {
	    f_to_s_max_I4(I4_X, nX, &i4val); 
	  }
#else
	  f_to_s_max_I4(I4_X, nX, &i4val); 
#endif
	  I4_part_rslt[tid] = i4val;
	  break;
	case I8 : 
	  I8_X = (long long *)X; I8_X += lb;
	  f_to_s_max_I8(((long long *)X)+lb, nX, &i8val); 
	  I8_part_rslt[tid] = i8val;
	  break;
	case F4 : 
	  F4_X = (float *)X; F4_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMax_32f(F4_X, nX, &f4val);
	  }
	  else {
	    f_to_s_max_F4(F4_X, nX, &f4val); 
	  }
#else
	  f_to_s_max_F4(F4_X, nX, &f4val); 
#endif
	  F4_part_rslt[tid] = f4val;
	  break;
	case F8 : 
	  F8_X = (double *)X; F8_X += lb;
#ifdef IPP
	  if ( nX < INT_MIN ) { 
	    ippsMax_64f(F8_X, nX, &f8val);
	  }
	  else {
	  f_to_s_max_F8(F8_X, nX, &f8val); 
	  }
#else
	  f_to_s_max_F8(F8_X, nX, &f8val); 
#endif
	  F8_part_rslt[tid] = f8val;
	  break;
	default : 
	  status = -1;
	  break;
	}
      }
      else {
	long long l_nn_cnt = 0;// no idea how many values are defined 
	switch ( fldtype ) {
	case I1 : 
	  nn_f_to_s_max_I1(((char *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i1val);
	  I1_part_rslt[tid] = i1val;
	  break;
	case I2 : 
	  nn_f_to_s_max_I2(((short *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i2val);
	  I2_part_rslt[tid] = i2val;
	  break;
	case I4 : 
	  nn_f_to_s_max_I4(((int *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i4val); 
	  I4_part_rslt[tid] = i4val;
	  break;
	case I8 : 
	  nn_f_to_s_max_I8(((long long *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &i8val);
	  I8_part_rslt[tid] = i8val;
	  break;
	case F4 : 
	  nn_f_to_s_max_F4(((float *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &f4val);
	  F4_part_rslt[tid] = f4val;
	  break;
	case F8 : 
	  nn_f_to_s_max_F8(((double *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &f8val);
	  F8_part_rslt[tid] = f8val;
	  break;
	default : 
	  status = -1;
	  break;
	}
        nn_cnt[tid] = l_nn_cnt;
      }
    }
    else if ( strcmp(op, "sum") == 0 ) {
      long long l_ll_numer = 0;
      double l_dd_numer = 0;
      if ( nn_X == NULL ) {
	nn_cnt[tid] = nX; // all values are defined 
	switch ( fldtype ) {
	case B  : 
	  status = f_to_s_sum_B(X, lb, ub, &l_ll_numer);
	  break;
	case I1 : 
	  f_to_s_sum_I1(((char *)X)+lb, nX, &l_ll_numer);
	  break;
	case I2 : 
	  f_to_s_sum_I2(((short *)X)+lb, nX, &l_ll_numer);
	  break;
	case I4 : 
	  f_to_s_sum_I4(((int *)X)+lb, nX, &l_ll_numer);
	  break;
	case I8 : 
	  f_to_s_sum_I8(((long long *)X)+lb, nX, &l_ll_numer);
	  break;
	case F4 : 
	  f_to_s_sum_F4(((float *)X)+lb, nX, &l_dd_numer);
	  break;
	case F8 : 
	  f_to_s_sum_F8(((double *)X)+lb, nX, &l_dd_numer);
	  break;
	default : 
	  status = -1;
	  break;
	}
	ll_numer[tid] = l_ll_numer;
	dd_numer[tid] = l_dd_numer;
      }
      else {
	long long l_nn_cnt = 0;// no idea how many values are defined 
      long long l_ll_numer = 0;
      double l_dd_numer = 0;
	switch ( fldtype ) {
	case I1 : 
	  nn_f_to_s_sum_I1(((char *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_ll_numer);
	  break;
	case I2 : 
	  nn_f_to_s_sum_I2(((short *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_ll_numer);
	  break;
	case I4 : 
	  nn_f_to_s_sum_I4(((int *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_ll_numer);
	  break;
	case I8 : 
	  nn_f_to_s_sum_I8(((long long *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_ll_numer);
	  break;
	case F4 : 
	  nn_f_to_s_sum_F4(((float *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_dd_numer);
	  break;
	case F8 : 
	  nn_f_to_s_sum_F8(((double *)X)+lb, nn_X+lb, nX, &l_nn_cnt, &l_dd_numer);
	  break;
	default : 
	  status = -1;
	  break;
	}
	nn_cnt[tid] = l_nn_cnt;
	ll_numer[tid] = l_ll_numer;
	dd_numer[tid] = l_dd_numer;
      }
    }
    else { 
      WHEREAMI; fprintf(stderr, "ERROR Bad operation = [%s] \n", op);
      status = -1; 
    }
  }
  cBYE(status);

  // Now combine results 
  if ( strcmp(op, "min") == 0 ) {
    switch ( fldtype ) { 
      case I1 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I1_part_rslt[i] < ll_minval ) { 
	    ll_minval = I1_part_rslt[i]; 
	  }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I2 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I2_part_rslt[i] < ll_minval ) { ll_minval = I2_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I4_part_rslt[i] < ll_minval ) { ll_minval = I4_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I8_part_rslt[i] < ll_minval ) { ll_minval = I8_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case F4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( F4_part_rslt[i] < dd_minval ) { dd_minval = F4_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_minval, cum_nn_cnt);
      break;
    case F8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( F8_part_rslt[i] < dd_minval ) { dd_minval = F8_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_minval, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else if ( strcmp(op, "max") == 0 ) {
    switch ( fldtype ) { 
    case I1 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I1_part_rslt[i] > ll_maxval ) { ll_maxval = I1_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I2 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I2_part_rslt[i] > ll_maxval ) { ll_maxval = I2_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I4_part_rslt[i] > ll_maxval ) { ll_maxval = I4_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( I8_part_rslt[i] > ll_maxval ) { ll_maxval = I8_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case F4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( F4_part_rslt[i] > dd_maxval ) { dd_maxval = F4_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_maxval, cum_nn_cnt);
      break;
    case F8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( F8_part_rslt[i] > dd_maxval ) { dd_maxval = F8_part_rslt[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_maxval, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else if ( strcmp(op, "sum") == 0 ) {
    f_to_s_sum_I8(nn_cnt, nT, &cum_nn_cnt);
    switch ( fldtype ) { 
    case B : 
    case I1 : 
    case I2 : 
    case I4 : 
    case I8 : 
      f_to_s_sum_I8(ll_numer, nT, &ll_sum);
      sprintf(rslt_buf, "%lld:%lld", ll_sum, cum_nn_cnt);
      break;
    case F4 : 
    case F8 : 
      f_to_s_sum_F8(dd_numer, nT, &dd_sum);
      sprintf(rslt_buf, "%lf:%lld", dd_sum, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
  }
  else {
    go_BYE(-1);
  }
 BYE:
  free_if_non_null(nn_cnt);
  free_if_non_null(ll_numer);
  free_if_non_null(dd_numer);
  free_if_non_null(part_rslt);
  return status ;
}
