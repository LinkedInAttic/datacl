#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "vec_f1opf2.h"
#include "get_sz_type_op_fld.h"
#include "mk_file.h"
#include "chunking.h"

#include "max_int.h"
#include "min_int.h"
#include "sum_int.h"

#include "max_longlong.h"
#include "min_longlong.h"
#include "sum_longlong.h"

#include "max_float.h"
#include "min_float.h"
#include "sum_float.h"

#include "max_bool.h"
#include "min_bool.h"
#include "sum_bool.h"

int g_thread_worked[MAX_NUM_THREADS];
int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS]; 
/* g_num_rows_processed is to make sure that exactly nR rows processed */
static int g_nT; /* num_threads */
static long long g_nR; /* number of rows to process */

static int   g_f1type;
static char *g_f1_X;
static char *g_nn_f1_X;
static char *g_op;
static char *g_str_result;

void *s_to_f_core(
		    void *arg
		    )
{
  int status = 0;
  char *g1_X = NULL; char *nn_g1_X = NULL;
  long long lb, lb1, lb2, ub, ub1, ub2;
  int num_rows_this_thread; /* number of rows for this thread to process */
  long long nB; /* number of blocks to process in inner loop */
  long long nX; /* number of rows handed to vector code */
  long long ll_numer = 0, ll_denom = 0; double dd_numer = 0;
  int imaxval = INT_MIN; long long llmaxval = LLONG_MIN; 
  char bmaxval = 0; float fmaxval = FLT_MIN;
  int iminval = INT_MAX; long long llminval = LLONG_MAX; 
  char bminval = 1; float fminval = FLT_MIN;
static int nC = CACHE_SIZE; /* number of rows to process at a time (cache aware) */

  int *iptr;
  iptr = (int *)arg;
  int tid = (int)(*iptr);
  char *my_op_X = NULL;
  // fprintf(stderr, "Hello World! It's me, thread # [%d]!\n", tid);
  if ( ( tid < 0 ) || ( tid > g_nT ) ) { go_BYE(-1); }
  g_thread_worked[tid] = TRUE;
  /* "De-reference global variables */
  long long nR = g_nR; 
  long long nT = g_nT;
  char *nn_f1_X = g_nn_f1_X;
  int f1type = g_f1type;
  char *f1_X = g_f1_X;
  char *op = g_op;
  char *op_X = g_op_X;

  for ( int t = 0; t < g_nT; t++ ) {
    if ( t != tid ) { continue; }
    status = get_outer_lb_ub(nR, nT, t, &lb1, &ub1);
    cBYE(status);
    num_rows_this_thread = ub1 - lb1;
    /* nR divisible by nT => some thread may have no work */
    if ( num_rows_this_thread <= 0 ) { continue; }
    status = get_num_blocks( num_rows_this_thread, nC, &nB);
    cBYE(status);
    for ( long long b = 0; b < nB; b++ ) {
      lb2 = b * nC;
      ub2 = lb2 + nC;
      if ( ub2 > num_rows_this_thread ) { ub2 = num_rows_this_thread; }
      cBYE(status);
      lb = lb1 + lb2;
      ub = lb1 + ub2;
      nX = ub2 - lb2;
      g_num_rows_processed[t] += nX;
      nn_g1_X = (nn_f1_X + ( sizeof(char) * lb ));
      if ( f1type == FLDTYPE_INT ) {
        g1_X = (f1_X + ( sizeof(int) * lb ));
	if ( strcmp(op, "max") == 0 ) {
	  max_int((int *)g1_X, nn_g1_X, nX, &imaxval);
	}
	else if ( strcmp(op, "min") == 0 ) {
	  min_int((int *)g1_X, nn_g1_X, nX,  &iminval);
	}
	else if ( ( strcmp(op, "sum") == 0 ) || ( strcmp(op, "avg") == 0) ) {
	  sum_int((int *)g1_X, nn_g1_X, nX,  &ll_numer, &ll_denom);
	}
	else { go_BYE(-1); }
      }
      else if (f1type == FLDTYPE_LONGLONG ) {
        g1_X = (f1_X + ( sizeof(long long) * lb ));
	if ( strcmp(op, "max") == 0 ) {
	  max_longlong((long long *)g1_X, nn_g1_X, nX,  &llmaxval);
	}
	else if ( strcmp(op, "min") == 0 ) {
	  min_longlong((long long *)g1_X, nn_g1_X, nX, &llminval);
	}
	else if ( ( strcmp(op, "sum") == 0 ) || ( strcmp(op, "avg") == 0) ) {
	  sum_longlong((long long *)g1_X, nn_g1_X, nX,  &ll_numer, &ll_denom);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_FLOAT ) {
        g1_X = (f1_X + ( sizeof(float) * lb ));
	if ( strcmp(op, "max") == 0 ) {
	  max_float((float *)g1_X, nn_g1_X, nX,  &fmaxval);
	}
	else if ( strcmp(op, "min") == 0 ) {
	  min_float((float *)g1_X, nn_g1_X, nX, &fminval);
	}
	else if ( ( strcmp(op, "sum") == 0 ) || ( strcmp(op, "avg") == 0) ) {
	  sum_float((float *)g1_X, nn_g1_X, nX,  &dd_numer, &ll_denom);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_BOOL ) {
        g1_X = (f1_X + ( sizeof(bool) * lb ));
	if ( strcmp(op, "max") == 0 ) {
	  max_bool((char *)g1_X, nn_g1_X, nX, &bmaxval);
	}
	else if ( strcmp(op, "min") == 0 ) {
	  min_bool((char *)g1_X, nn_g1_X, nX,  &bminval);
	}
	else if ( strcmp(op, "sum") == 0 ) {
	  sum_bool((char *)g1_X, nn_g1_X, nX,  &ll_numer, &ll_denom);
	}
	else { go_BYE(-1); }
      }
    }
    partial_imaxval[t]  = imaxval; 
    partial_llmaxval[t] = llmaxval; 
    partial_bmaxval[t]  = bmaxval; 
    partial_fmaxval[t]  = fmaxval; 
    partial_iminval[t]  = iminval; 
    partial_llminval[t] = llminval; 
    partial_bminval[t]  = bminval; 
    partial_fminval[t]  = fminval; 
    partial_ll_numer[t]  = ll_numer; 
    partial_dd_numer[t]  = dd_numer; 
    partial_ll_denom[t]  = ll_denom; 
  }
 BYE:
  if ( g_nT == 1 ) {
    if ( status == 0 ) {
      return ((void *)0);
    }
    else {
      return ((void *)1);
    }
  }
  pthread_exit(NULL);
}


//---------------------------------------------------------------
// START FUNC DECL 
int 
vec_s_to_f(
    char *in_f1_X,
	     long long in_nR,
	     char *in_op,
	     char *opfile
	     )
// STOP FUNC DECL 
{
  int status = 0;
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  char *endptr = NULL;
  long long chk_nR = 0; 
  
  nR = in_nR;
  f1_X   = in_f1_X;
  op     = in_op;
  str_result = in_str_result;

  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    partial_dd_numer[i] = partial_ll_denom[i] = partial_ll_numer[i] = 0;
  }
  dd_numer = ll_denom = ll_numer = 0;

  /*--------------------------------------------*/
  /* Set up values needed for core routine */
  g_nR       = in_nR;
  g_f1type   = in_f1type;
  g_f1_X     = in_f1_X;
  g_nn_f1_X  = in_nn_f1_X;
  g_sz_f1_X  = in_sz_f1_X;
  g_op       = in_op;
  g_op_X     = op_X;

  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_worked[i] = FALSE;
    g_thread_id[i] = i;
    g_num_rows_processed[i] = 0;
  }
#define MIN_ROWS_FOR_PARALLELISM_F_TO_S 4 // 1048576 
  if ( nR <= MIN_ROWS_FOR_PARALLELISM_F_TO_S ) {
    g_nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( g_nT > nR ) { g_nT = nR; }

  if ( g_nT == 1 ) { 
    void *vstat;
    vstat = f_to_s_core(&(g_thread_id[0]));
    chk_nR = g_num_rows_processed[0];
  }
  else {
    // Create threads
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, f_to_s_core, &(g_thread_id[t]));
      if ( rc ) {
	fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
	go_BYE(-1);
      }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_nR += g_num_rows_processed[t];

    }
  }
  if ( chk_nR != in_nR ) { go_BYE(-1); }
  // Compute final result from partial results
  for ( int t = 0; t < nT; t++ ) { 
    final_imaxval  = max(final_imaxval,  partial_imaxval[t]); 
    final_llmaxval = max(final_llmaxval, partial_llmaxval[t]);
    final_bmaxval  = max(final_bmaxval,  partial_bmaxval[t]);
    final_fmaxval  = max(final_fmaxval,  partial_fmaxval[t]);
    final_iminval  = min(final_iminval,  partial_iminval[t]);
    final_llminval = min(final_llminval, partial_llminval[t]);
    final_bminval  = min(final_bminval,  partial_bminval[t]);
    final_fminval  = min(final_fminval,  partial_fminval[t]);
    dd_numer += partial_dd_numer[t];
    ll_numer += partial_ll_numer[t];
    ll_denom += partial_ll_denom[t];
  }
  // Print out desired final result
  switch ( f1type ) { 
    case FLDTYPE_INT : 
      if ( strcmp(op, "min") == 0 ) {
	sprintf(str_result, "%d", final_iminval);
      }
      else if ( strcmp(op, "max") == 0 ) {
	sprintf(str_result, "%d", final_imaxval);
      }
      else if ( strcmp(op, "avg") == 0 ) {
	sprintf(str_result, "%lf", ll_numer / (double)ll_denom);
      }
      else if ( strcmp(op, "sum") == 0 ) {
	sprintf(str_result, "%lld", ll_numer);
      }
      else { go_BYE(-1);
      }
      break;
    case FLDTYPE_LONGLONG : 
      if ( strcmp(op, "min") == 0 ) {
	sprintf(str_result, "%lld", final_llminval);
      }
      else if ( strcmp(op, "max") == 0 ) {
	sprintf(str_result, "%lld", final_llmaxval);
      }
      else if ( strcmp(op, "avg") == 0 ) {
	sprintf(str_result, "%lf", ll_numer / (double)ll_denom);
      }
      else if ( strcmp(op, "sum") == 0 ) {
	sprintf(str_result, "%lld", ll_numer);
      }
      else { go_BYE(-1);
      }
      break;
    case FLDTYPE_FLOAT : 
      if ( strcmp(op, "min") == 0 ) {
	sprintf(str_result, "%f", final_fminval);
      }
      else if ( strcmp(op, "max") == 0 ) {
	sprintf(str_result, "%f", final_fmaxval);
      }
      else if ( strcmp(op, "avg") == 0 ) {
	sprintf(str_result, "%lf", dd_numer / (double)ll_denom);
      }
      else if ( strcmp(op, "sum") == 0 ) {
	sprintf(str_result, "%lld", ll_numer);
      }
      else { go_BYE(-1);
      }
      break;
    case FLDTYPE_BOOL : 
      if ( strcmp(op, "min") == 0 ) {
	sprintf(str_result, "%d", final_bminval);
      }
      else if ( strcmp(op, "max") == 0 ) {
	sprintf(str_result, "%d", final_bmaxval);
      }
      else if ( strcmp(op, "sum") == 0 ) {
	sprintf(str_result, "%lld", ll_numer);
      }
      else { go_BYE(-1);
      }
      break;
    default : 
      go_BYE(-1);
      break;
  }


  //--------------------------------------------
 BYE:
  return(status);
}
