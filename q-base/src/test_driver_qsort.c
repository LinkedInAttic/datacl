#include "qtypes.h"

#include "qsort_with_idx.h"
#include "qsort_with_idx_int.h"
#include "qsort_with_idx_longlong.h"
#include "qsort_dsc_I4.h"
#include "qsort_asc_I4.h"
#include "qsort_asc_I8.h"
#include "qsort_dsc_I8.h"
#include "sort_asc_I4.h"
#include "sort_dsc_I4.h"
#include "sort_asc_I8.h"
#include "sort_dsc_I8.h"
#include "qsort_asc_val_I4_idx_I4.h"
#include "qsort_asc_val_I4_idx_I8.h"
#include "qsort_asc_val_I8_idx_I4.h"
#include "qsort_asc_val_I8_idx_I8.h"

/* DO NOT AUTO GENERATE HEADER FILE  */
void
set_vals_int(
	     int X[],
	     int N
	     )
{
  for ( int i = 0; i < N; i++ ) { 
    X[i] = (int)mrand48();
  }
  /*
  if ( N == 16 ) { 
    X[0]  = 0; 
    X[1]  = 10; 
    X[2]  = 200; 
    X[3]  = 3000; 
    X[4]  = 40000; 
    X[5]  = 5000; 
    X[6]  = 600; 
    X[7]  = 70; 
    X[8]  = 8; 
    X[9]  = -90; 
    X[10] = -1000; 
    X[11] = -11000; 
    X[12] = -120000; 
    X[13] = -13000; 
    X[14] = -14000; 
    X[15] = -150; 
  }
  */
}

void
set_vals_longlong(
		  long long X[],
		  int N
		  )
{
  long long minval = LLONG_MAX, maxval = LLONG_MIN;
  for ( int i = 0; i < N; i++ ) { 
    X[i] = (long long) mrand48();
    if ( X[i] < 0 ) { 
      X[i] = -1 * X[i] * X[i]; 
    }
    else  {
      X[i] =  1 * X[i] * X[i]; 
    }
    if ( X[i] < minval ) { minval = X[i]; }
    if ( X[i] > maxval ) { maxval = X[i]; }
  }
  // fprintf(stderr, "min = %lld, max = %lld \n", minval, maxval);
}

int
main()
{
  int status = 0;
#define N 1*1048576
#define ITERS 12*64
  int *X = NULL; int *Y = NULL; int *Z = NULL;
  int *i_srt_ordr = NULL;
  long long *ll_srt_ordr = NULL;
  long long *XLL = NULL; long long *YLL = NULL; long long *ZLL = NULL;
  // START FOR TIMING 
  struct timeval *Tps = NULL, *Tpf = NULL;
  void *Tzp = NULL;
  unsigned long long int t_before_sec = 0, t_before_usec = 0, t_before = 0;
  unsigned long long int t_after_sec, t_after_usec, t_after;
  unsigned long long int t_regular, t_optimized;
  // STOP FOR TIMING 

  X          = (int *)malloc(N * sizeof(int));
  Y          = (int *)malloc(N * sizeof(int));
  Z          = (int *)malloc(N * sizeof(int));
  i_srt_ordr = (int *)malloc(N * sizeof(int));

  ll_srt_ordr = (long long *)malloc(N * sizeof(long long));
  XLL         = (long long *)malloc(N * sizeof(long long));
  YLL         = (long long *)malloc(N * sizeof(long long));
  ZLL         = (long long *)malloc(N * sizeof(long long));

  for ( int i = 0; i < N; i++  ) { i_srt_ordr[i] = i;  }
  for ( int i = 0; i < N; i++  ) { ll_srt_ordr[i] = i;  }

  //-------------------------------------------
  Tps = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tps == NULL ) { go_BYE(-1); }
  Tpf = (struct timeval *) malloc(sizeof(struct timeval));
  if ( Tpf == NULL ) { go_BYE(-1); }
  Tzp = 0;
  //-------------------------------------------
  gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
  t_before_sec  = (long long int)Tps->tv_sec;
  t_before_usec = (long long int)Tps->tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;
  srand48(t_before_usec);
  //-------------------------------------------

  for ( int iters = 0; iters < ITERS; iters++ ) {
    int itype = iters % 12;
    switch ( itype ) { 
      case 0  : fprintf(stderr, "Regular sort              --"); break; 
      case 1  : fprintf(stderr, "Sort with integer index   --"); break; 
      case 2  : fprintf(stderr, "Sort with long long index --"); break; 
      case 3  : fprintf(stderr, "Sort asc int no func      --"); break; 
      case 4  : fprintf(stderr, "Sort asc int no func      --"); break; 
      case 5  : fprintf(stderr, "Sort dsc int no func      --"); break; 
      case 6  : fprintf(stderr, "Sort asc longlong no func --"); break; 
      case 7  : fprintf(stderr, "Sort dsc longlong no func --"); break; 
      case 8  : fprintf(stderr, "Sort asc val int idx int  --"); break; 
      case 9  : fprintf(stderr, "Sort asc val int idx LL   --"); break; 
      case 10 : fprintf(stderr, "Sort asc val LL  idx int  --"); break; 
      case 11 : fprintf(stderr, "Sort asc val LL  idx LL   --"); break; 
      default : go_BYE(-1); break;
    }
    //---------------------------------------------------
    // set vals
    set_vals_int(X, N);
    set_vals_longlong(XLL, N);
    for ( int i = 0; i < N; i++ ) { Y[i] = X[i]; }
    for ( int i = 0; i < N; i++ ) { Z[i] = X[i]; }
    for ( int i = 0; i < N; i++  ) { i_srt_ordr[i] = i; }
    for ( int i = 0; i < N; i++  ) { ll_srt_ordr[i] = i; }
    for ( int i = 0; i < N; i++ ) { YLL[i] = XLL[i]; }
    for ( int i = 0; i < N; i++ ) { ZLL[i] = XLL[i]; }
    //---------------------------------------------------
    // sort regular way 
    gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
    t_before_sec  = (long long int)Tps->tv_sec;
    t_before_usec = (long long int)Tps->tv_usec;
    t_before = t_before_sec * 1000000 + t_before_usec;
    switch ( itype ) { 
      case 0 : 
      case 1 : 
      case 2 : 
      case 4 : 
        qsort(Z, N, sizeof(int), sort_asc_I4);
	break;
      case 5 : 
        qsort(Z, N, sizeof(int), sort_dsc_I4);
	break;
      case 6 : 
        qsort(ZLL, N, sizeof(long long), sort_asc_I8);
	break;
      case 7 : 
        qsort(ZLL, N, sizeof(long long), sort_dsc_I8);
	break;
      case 8 : 
      case 9 : 
        qsort(Z, N, sizeof(int), sort_asc_I4);
	break;
      case 10 : 
      case 11 : 
        qsort(ZLL, N, sizeof(long long), sort_asc_I8);
	break;
      default : 
	go_BYE(-1);
	break;
    }
    gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
    t_after_sec  = (long long int)Tps->tv_sec;
    t_after_usec = (long long int)Tps->tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    t_regular = t_after - t_before;
    //---------------------------------------------------
    // sort optimized
    gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
    t_before_sec  = (long long int)Tps->tv_sec;
    t_before_usec = (long long int)Tps->tv_usec;
    t_before = t_before_sec * 1000000 + t_before_usec;
    switch ( itype ) { 
      case 0 : 
        qsort_with_idx(i_srt_ordr, Y, N, sizeof(int), sort_asc_I4);
	break;
      case 1 : 
        qsort_with_idx_int(i_srt_ordr, Y, N, sizeof(int), sort_asc_I4);
	break;
      case 2 : 
        qsort_with_idx_longlong(ll_srt_ordr, Y, N, sizeof(int), sort_asc_I4);
	break;
      case 4 : 
        qsort_asc_I4(Y, N, sizeof(int), NULL);
	break;
      case 5 : 
        qsort_dsc_I4(Y, N, sizeof(int), NULL);
	break;
      case 6 : 
        qsort_asc_I8(YLL, N, sizeof(long long), NULL);
	break;
      case 7 : 
        qsort_dsc_I8(YLL, N, sizeof(long long), NULL);
	break;
      case 8 : 
        qsort_asc_val_I4_idx_I4(i_srt_ordr, (void *)Y, N, NULL);
	break;
      case 9 : 
        qsort_asc_val_I4_idx_I8(ll_srt_ordr, Y, N, NULL);
	break;
      case 10 : 
        qsort_asc_val_I8_idx_I4(i_srt_ordr, YLL, N, NULL);
	break;
      case 11 : 
        qsort_asc_val_I8_idx_I8(ll_srt_ordr, YLL, N, NULL);
	break;
      default : 
	go_BYE(-1);
	break;
    }
    gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
    t_after_sec  = (long long int)Tps->tv_sec;
    t_after_usec = (long long int)Tps->tv_usec;
    t_after = t_after_sec * 1000000 + t_after_usec;
    t_optimized = t_after - t_before;
    //---------------------------------------------------
    // check 
    for ( int i = 0; i < N; i++ ) { 
      long long val1, val2;
      switch ( itype ) { 
	case 0 :
	case 1 : 
        val1 = Y[i];
        val2 = X[i_srt_ordr[i]];
	break;
	case 2 :
        val1 = Y[i];
        val2 = X[ll_srt_ordr[i]];
	break;
	case 4 : 
	case 5 : 
        val1 = Y[i];
        val2 = Z[i];
	break;
	case 6 : 
	case 7 : 
        val1 = YLL[i];
	val2 = ZLL[i];
	break;
	case 8 : 
        val1 = Y[i];
        val2 = X[i_srt_ordr[i]];
	break;
	case 9 : 
        val1 = Y[i];
        val2 = X[ll_srt_ordr[i]];
	break;
	case 10 : 
        val1 = YLL[i];
        val2 = XLL[i_srt_ordr[i]];
	break;
	case 11 : 
        val1 = YLL[i];
        val2 = XLL[ll_srt_ordr[i]];
	break;
	default : 
	go_BYE(-1);
	break;
      }
      if ( val1 != val2 ) { 
	fprintf(stderr, "mismatch at pos %d \n", i);
        for ( int j = i; (( j < N ) && ( j < i + 10 )); j++ ) { 
	  fprintf(stderr,  "%d: good =  %d, bad = %d \n", j, Z[j], Y[j]);
	}
	go_BYE(-1);
      }
    }
    //---------------------------------------------------
    fprintf(stderr, " t_reg = %lld, t_opt = %lld \n", t_regular, t_optimized);
  }

 BYE:
  free_if_non_null(i_srt_ordr);
  free_if_non_null(ll_srt_ordr);
  free_if_non_null(X);
  free_if_non_null(Y);
  free_if_non_null(Z);
  free_if_non_null(XLL);
  free_if_non_null(YLL);
  free_if_non_null(ZLL);
  free_if_non_null(Tps);
  free_if_non_null(Tpf);
  return(status);
}
