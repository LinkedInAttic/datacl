#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "sqlite3.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "vec_f1s1opf2.h"
#include "get_sz_type_op_fld.h"
#include "mk_file.h"
#include "chunking.h"

#include "add_scalar_int.h"
#include "sub_scalar_int.h"
#include "mul_scalar_int.h"
#include "div_scalar_int.h"
#include "rem_scalar_int.h"
#include "add_scalar_longlong.h"
#include "sub_scalar_longlong.h"
#include "mul_scalar_longlong.h"
#include "div_scalar_longlong.h"
#include "rem_scalar_longlong.h"
#include "add_scalar_float.h"
#include "sub_scalar_float.h"
#include "mul_scalar_float.h"
#include "div_scalar_float.h"
#include "and_scalar_bool.h"
#include "or_scalar_bool.h"
#include "bwise_and_scalar_int.h"
#include "bwise_and_scalar_longlong.h"
#include "bwise_and_scalar_bool.h"
#include "bwise_or_scalar_int.h"
#include "bwise_or_scalar_longlong.h"
#include "bwise_or_scalar_bool.h"
#include "bwise_xor_scalar_int.h"
#include "bwise_xor_scalar_longlong.h"
#include "bwise_xor_scalar_bool.h"

#include "shift_left_scalar_int.h"
#include "shift_left_scalar_longlong.h"
#include "shift_right_scalar_int.h"
#include "shift_right_scalar_longlong.h"

#include "cmp_le_scalar_int.h"
#include "cmp_ge_scalar_int.h" 
#include "cmp_eq_scalar_int.h"
#include "cmp_ne_scalar_int.h"
#include "cmp_gt_scalar_int.h"
#include "cmp_lt_scalar_int.h"
#include "cmp_le_scalar_longlong.h"
#include "cmp_ge_scalar_longlong.h"
#include "cmp_eq_scalar_longlong.h"
#include "cmp_ne_scalar_longlong.h"
#include "cmp_lt_scalar_longlong.h"
#include "cmp_gt_scalar_longlong.h"
#include "cmp_le_scalar_float.h" 
#include "cmp_ge_scalar_float.h"
#include "cmp_eq_scalar_float.h"
#include "cmp_ne_scalar_float.h"
#include "cmp_lt_scalar_float.h"
#include "cmp_gt_scalar_float.h"
#include "cmp_le_scalar_bool.h"
#include "cmp_ge_scalar_bool.h"
#include "cmp_eq_scalar_bool.h"
#include "cmp_ne_scalar_bool.h"
#include "cmp_lt_scalar_bool.h"
#include "cmp_gt_scalar_bool.h"

#include "cmp_eq_mult_scalar_int.h"

#include "assign_char.h"
#include "sort_asc_int.h"

static int nT; /* num_threads */
static long long nR; /* number of rows to process */
static int nC = CACHE_SIZE; /* number of rows to process at a time (cache aware) */
static char *op_X = NULL; size_t n_op_X; /* for mmaped output file */
static char *nn_op_X = NULL; size_t n_nn_op_X; /* for mmaped nn file */
static int   f1type;
static char *f1_X;
static char *nn_f1_X;
static char *op;
static bool bval;  // For scalar value 
static int ival;  // For scalar value 
static long long llval;  // For scalar value 
static float fval; // For scalar value 

static int *ivals = NULL;  // For multiple scalar values
static int n_ivals = 0;  // Number of multiple scalar values

void *f1s1opf2_core(
		    void *threadid
		    )
{
  int status = 0;
  long tid;
  char *g_op_X = NULL;
  char *g1_X;
  long long lb, lb1, lb2, ub, ub1, ub2;
  int num_rows_this_thread; /* number of rows for this thread to process */
  long long nB; /* number of blocks to process in inner loop */
  int nX; /* number of rows handed to vector code */
  int *X;

  tid = (long)threadid;
  // printf("Hello World! It's me, thread #%ld!\n", tid);
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    status = get_outer_lb_ub(nR, nT, t, &lb1, &ub1);
    cBYE(status);
    num_rows_this_thread = ub1 - lb1;
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
      /*
	fprintf(stderr, "Thread %ld processing %lld rows from [%lld, %lld] \n", 
	tid, (ub - lb), lb, ub);
      */
      if ( f1type == FLDTYPE_INT ) {
        g1_X = (f1_X + ( sizeof(int) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  g_op_X = (op_X + ( sizeof(int) * lb ));
	  add_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  sub_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  mul_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  div_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "%") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  rem_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "<<") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  shift_left_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	  X = (int *)g_op_X;
	}
	else if ( strcmp(op, ">>") == 0 ) {
          g_op_X = (op_X + ( sizeof(int) * lb ));
	  shift_right_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  if ( ivals == NULL ) { /* single scalar value */
	    cmp_eq_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	  }
	  else {
	    cmp_eq_mult_scalar_int((int *)g1_X, nX, ivals, n_ivals, (char *)g_op_X);
	  }
	}
	else if ( strcmp(op, "!=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_int((int *)g1_X, ival, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "&") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  bwise_and_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  bwise_or_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  bwise_xor_scalar_int((int *)g1_X, ival, nX, (int *)g_op_X);
	}
	else { go_BYE(-1); }
      }
      else if (f1type == FLDTYPE_LONGLONG ) {
        g1_X = (f1_X + ( sizeof(long long) * lb ));
        g_op_X = (op_X + ( sizeof(long long) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  add_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "%") == 0 ) {
	  rem_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "<<") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  shift_left_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, ">>") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  shift_right_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_ge_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_le_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_eq_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_ne_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_gt_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  cmp_lt_scalar_longlong((long long *)g1_X, llval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "&") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_and_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_or_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
          g_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_xor_scalar_longlong((long long *)g1_X, llval, nX, (long long *)g_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_FLOAT ) {
        g1_X = (f1_X + ( sizeof(float) * lb ));
        g_op_X = (op_X + ( sizeof(float) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  add_scalar_float((float *)g1_X, fval, nX, (float *)g_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub_scalar_float((float *)g1_X, fval, nX, (float *)g_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul_scalar_float((float *)g1_X, fval, nX, (float *)g_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div_scalar_float((float *)g1_X, fval, nX, (float *)g_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_float((float *)g1_X, fval, nX, (char *)g_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_BOOL ) {
        g1_X = (f1_X + ( sizeof(bool) * lb ));
        g_op_X = (op_X + ( sizeof(bool) * lb ));
	if ( strcmp(op, "&") == 0 ) {
	  bwise_and_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
	  bwise_or_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
	  bwise_xor_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          g_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_bool((char *)g1_X, bval, nX, (char *)g_op_X);
	}
	else { go_BYE(-1); }
      }
    }
  }
 BYE:
  if ( nT == 1 ) {
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
vec_f1s1opf2(
	     long long in_nR,
	     int in_f1type,
	     char *str_meta_data,
	     char *in_f1_X,
	     char *in_nn_f1_X,
	     char *str_scalar,
	     char *in_op,
	     char **ptr_opfile,
	     char **ptr_nn_opfile
	     )
// STOP FUNC DECL 
{
  int status = 0;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;
  int sizeof_op; /* sizeof each cell of output */
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  char *endptr = NULL;
  FLD_META_TYPE out_fld_meta; int optype;
  char **str_ivals = NULL;
  bool is_multiple = false;
  
  nR = in_nR;
  f1type = in_f1type;
  f1_X   = in_f1_X;
  nn_f1_X   = in_nn_f1_X;
  op     = in_op;

  //--------------------------------------------
  switch ( f1type ) { 
    case FLDTYPE_INT : 
      for ( char *cptr = str_scalar; *cptr != '\0'; cptr++ ) { 
	if ( *cptr == ':' ) {
	  is_multiple = true;
	  break;
	}
      }
      if ( !is_multiple ) { 
        ival = strtol(str_scalar, &endptr, 10);
        if ( *endptr != '\0' ) { go_BYE(-1); }
      }
      else {
	status = break_str(str_scalar, ":", &str_ivals, &n_ivals);
	cBYE(status);
	ivals = (int *)malloc(n_ivals * sizeof(int));
	return_if_malloc_failed(ivals);
	for ( int i = 0; i < n_ivals; i++ ) { 
          ivals[i] = strtol(str_ivals[i], &endptr, 10);
          if ( *endptr != '\0' ) { go_BYE(-1); }
	  free_if_non_null(str_ivals[i]);
	}
        free_if_non_null(str_ivals);
	qsort(ivals, n_ivals, sizeof(int), sort_asc_int);

	


      }
      break;
    case FLDTYPE_LONGLONG : 
      llval = strtoll(str_scalar, &endptr, 10);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      break;
    case FLDTYPE_FLOAT : 
      fval = strtof(str_scalar, &endptr);
      if ( *endptr != '\0' ) { go_BYE(-1); }
      break;
    case FLDTYPE_BOOL : 
      if ( ( strcasecmp(str_scalar, "true") == 0 ) || 
	  ( strcmp(str_scalar, "1") == 0 ) ) {
	bval = true;
      }
      else if ( ( strcasecmp(str_scalar, "fals") == 0 ) || 
	  ( strcmp(str_scalar, "0") == 0 ) ) {
	bval = true;
      }
      else { go_BYE(-1); }
    default :
      go_BYE(-1);
      break;
  }
  //--------------------------------------------
  // create an output file and mmap it 
  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  fclose_if_non_null(ofp);
  status = get_sz_type_op_fld("f1s1opf2", op, f1type, f1type, "", &optype, 
      &sizeof_op);
  cBYE(status);
  //------------------------------------------
  zero_fld_meta(&out_fld_meta);
  strcpy(out_fld_meta.filename, opfile);
  status = mk_str_fldtype(optype, out_fld_meta.fldtype);
  cBYE(status);
  out_fld_meta.n_sizeof = sizeof_op;
  status = fld_meta_to_str(out_fld_meta, str_meta_data);
  cBYE(status);

  //------------------------------------------
  n_op_X = sizeof_op * nR;
  status = mk_file(opfile, n_op_X);
  cBYE(status);
  status = rs_mmap(opfile, &op_X, &n_op_X, 1);
  cBYE(status);
  //--------------------------------------------
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
  if ( in_nn_f1_X == NULL ) {
    /* No need for nn file */
  }
  else {
    status = open_temp_file(&nn_ofp, &nn_opfile);
    cBYE(status);
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, nR * sizeof(char));
    rs_mmap(nn_opfile, &nn_op_X, &n_nn_op_X, 1);
    assign_char(nn_op_X, in_nn_f1_X, nR);
    rs_munmap(nn_op_X, n_nn_op_X);
  }
  //--------------------------------------------
  if ( ( nR <= MIN_ROWS_FOR_PARALLELISM ) || ( nT == 1 ) ) { 
    /* if one thread or if not enough data to justify threads */
    // fprintf(stderr, "nT = 1 or nR = [%lld] too small for parallelism \n", nR);
    nT = 1;
    void *vstat;
    vstat = f1s1opf2_core(0);
  }
  else {
    // Create threads
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, f1s1opf2_core, (void *)t);
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
    }
  }
  //--------------------------------------------
  *ptr_opfile = opfile;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  free_if_non_null(ivals);
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  return(status);
}
