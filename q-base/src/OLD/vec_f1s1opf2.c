#include <stdio.h> 
#include <unistd.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
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

#include "f1opf2_cum.h"
#include "cmp_eq_scalar_char_string.h"

#include "assign_char.h"
#include "sort_asc_int.h"

int g_thread_worked[MAX_NUM_THREADS];
int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS]; 
/* g_num_rows_processed is to make sure that exactly nR rows processed */
static int g_nT; /* num_threads */
static long long g_nR; /* number of rows to process */
static char *g_op_X = NULL; 
static int   g_f1type;
static char *g_f1_X;
static char *g_nn_f1_X;
static char *g_sz_f1_X;
static long long *offset_X;
static char *g_op; /* operation to be performed */
static bool bval;  // For scalar value 
static int ival;  // For scalar value 
static long long llval;  // For scalar value 
static float fval; // For scalar value 
static char *g_str_scalar; // For scalar value string

static int *ivals = NULL;  // For multiple scalar values
static int n_ivals = 0;  // Number of multiple scalar values


void *f1s1opf2_core(
		    void *arg
		    )
{
  int status = 0;
  char *g1_X, *nn_g1_X = NULL, *sz_g1_X = NULL;
  long long lb, lb1, lb2, ub, ub1, ub2;
  int num_rows_this_thread; /* number of rows for this thread to process */
  long long nB; /* number of blocks to process in inner loop */
  long long nX; /* number of rows handed to vector code */
  int *X;
  int nC = CACHE_SIZE; /* number of rows to process at a time (cache aware) */

  int *iptr;
  iptr = (int *)arg;
  int tid = (int)(*iptr);
  char *my_op_X = NULL;
  // fprintf(stderr, "Hello World! It's me, thread # [%d]!\n", tid);
  /* "De-reference global variables */
  long long nR = g_nR; 
  long long nT = g_nT; 
  char *op_X = g_op_X;
  char *op = g_op;
  char *nn_f1_X = g_nn_f1_X;
  char *sz_f1_X = g_sz_f1_X;
  char *f1_X    = g_f1_X;
  int f1type = g_f1type;
  /*-----------------------------------------------------*/
  for ( int t = 0; t < nT; t++ ) {
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
      /*
	fprintf(stderr, "Thread %ld processing %lld rows from [%lld, %lld] \n", 
	tid, (ub - lb), lb, ub);
      */
      if ( f1type == FLDTYPE_INT ) {
        g1_X = (f1_X + ( sizeof(int) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  my_op_X = (op_X + ( sizeof(int) * lb ));
	  add_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  sub_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  mul_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  div_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "%") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  rem_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "<<") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  shift_left_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	  X = (int *)my_op_X;
	}
	else if ( strcmp(op, ">>") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  shift_right_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  if ( ivals == NULL ) { /* single scalar value */
	    cmp_eq_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	  }
	  else {
	    cmp_eq_mult_scalar_int((int *)g1_X, nX, ivals, n_ivals, (char *)my_op_X);
	  }
	}
	else if ( strcmp(op, "!=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_int((int *)g1_X, ival, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_and_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_or_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bwise_xor_scalar_int((int *)g1_X, ival, nX, (int *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if (f1type == FLDTYPE_LONGLONG ) {
        g1_X = (f1_X + ( sizeof(long long) * lb ));
        my_op_X = (op_X + ( sizeof(long long) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  add_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "%") == 0 ) {
	  rem_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "<<") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  shift_left_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, ">>") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  shift_right_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_longlong((long long *)g1_X, llval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "&") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_and_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_or_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  bwise_xor_scalar_longlong((long long *)g1_X, llval, nX, (long long *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_FLOAT ) {
        g1_X = (f1_X + ( sizeof(float) * lb ));
        my_op_X = (op_X + ( sizeof(float) * lb ));
	if ( strcmp(op, "+") == 0 ) {
	  add_scalar_float((float *)g1_X, fval, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, "-") == 0 ) {
	  sub_scalar_float((float *)g1_X, fval, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, "*") == 0 ) {
	  mul_scalar_float((float *)g1_X, fval, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, "/") == 0 ) {
	  div_scalar_float((float *)g1_X, fval, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_float((float *)g1_X, fval, nX, (char *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_BOOL ) {
        g1_X = (f1_X + ( sizeof(bool) * lb ));
        my_op_X = (op_X + ( sizeof(bool) * lb ));
	if ( strcmp(op, "&") == 0 ) {
	  bwise_and_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "|") == 0 ) {
	  bwise_or_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "^") == 0 ) {
	  bwise_xor_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ge_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_le_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "==") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "!=") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_ne_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, ">") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_gt_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "<") == 0 ) {
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_lt_scalar_bool((char *)g1_X, bval, nX, (char *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_CHAR_STRING ) { 
	if ( strcmp(op, "==") == 0 ) {
	  /* The reason for the awkwardness below is that offset is not
	   * *really* an offset but a cum applied to sz. So, it doesn't
	   * start as 0 as we would like it to have */
	  if ( lb == 0 ) { 
	    g1_X = f1_X;
	  }
	  else {
	    g1_X = f1_X + offset_X[lb-1];
	  }
	  /*--------------------------------------------------*/
	  if ( nn_f1_X == NULL ) { 
	    nn_g1_X = NULL;
	  }
	  else {
  	    nn_g1_X = nn_f1_X + (sizeof(char) * lb );
	  }
	  sz_g1_X = sz_f1_X + (sizeof(int)  * lb );
          my_op_X = (op_X + ( sizeof(char) * lb ));
	  cmp_eq_scalar_char_string(g1_X, nn_g1_X, (int *)sz_g1_X, 
	      nX, g_str_scalar, (bool *)my_op_X);
	}
	else {
	  go_BYE(-1);
	}
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
	     char *in_sz_f1_X,
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
  FLD_TYPE out_fld_meta; int optype;
  char **str_ivals = NULL;
  bool is_multiple = false;
  char *cum_opfile = NULL;
  char *cum_X = NULL; size_t cum_nX = 0;
  char *op_X = NULL; size_t n_op_X = 0; /* for mmaped output file */
  char *nn_op_X = NULL; size_t n_nn_op_X = 0; /* for mmaped nn file */
  long long chk_nR = 0;
  
  g_nR = in_nR;
  g_f1type = in_f1type;
  g_f1_X   = in_f1_X;
  g_nn_f1_X   = in_nn_f1_X;
  g_sz_f1_X   = in_sz_f1_X;
  g_op     = in_op;
  g_str_scalar = str_scalar;
  //--------------------------------------------
  switch ( g_f1type ) { 
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
      else if ( ( strcasecmp(str_scalar, "false") == 0 ) || 
	  ( strcmp(str_scalar, "0") == 0 ) ) {
	bval = true;
      }
      else { go_BYE(-1); }
    case FLDTYPE_CHAR_STRING : 
      /* Convert size into an offset */
      status = f1opf2_cum(in_sz_f1_X, in_nR, FLDTYPE_INT,
	  FLDTYPE_LONGLONG, false, 0, 0, &cum_opfile);
      cBYE(status);
      status = rs_mmap(cum_opfile, &cum_X, &cum_nX, 0);
      cBYE(status);
      offset_X = (long long *)cum_X;
      break;
    default :
      go_BYE(-1);
      break;
  }
  //--------------------------------------------
  // create an output file and mmap it 
  status = open_temp_file(&ofp, &opfile, 0); cBYE(status);
  fclose_if_non_null(ofp);
  status = get_sz_type_op_fld("f1s1opf2", g_op, g_f1type, g_f1type, 
      "", &optype, &sizeof_op);
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
  n_op_X = sizeof_op * g_nR;
  status = mk_file(opfile, n_op_X); cBYE(status);
  status = rs_mmap(opfile, &op_X, &n_op_X, 1); cBYE(status);
  //--------------------------------------------
  if ( in_nn_f1_X == NULL ) {
    /* No need for nn file */
  }
  else {
    status = open_temp_file(&nn_ofp, &nn_opfile, 0); cBYE(status);
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, g_nR * sizeof(char));
    rs_mmap(nn_opfile, &nn_op_X, &n_nn_op_X, 1);
    assign_char(nn_op_X, in_nn_f1_X, g_nR);
    rs_munmap(nn_op_X, n_nn_op_X);
  }
  //--------------------------------------------
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
  status = get_num_threads(&g_nT);
  cBYE(status);
#define MIN_ROWS_FOR_PARALLELISM_F1S1OPF2 4 // 65536
  if ( g_nR <= MIN_ROWS_FOR_PARALLELISM_F1S1OPF2 ) {
    g_nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( g_nT > in_nR ) { g_nT = in_nR; }

  if ( g_nT == 1 ) { 
    void *vstat;
    vstat = f1s1opf2_core(&(g_thread_id[0]));
    chk_nR = g_num_rows_processed[0];
  }
  else {
    // Create threads
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, f1s1opf2_core,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_nR += g_num_rows_processed[t];
    }
  }
  if ( chk_nR != in_nR ) { go_BYE(-1); }
  //--------------------------------------------
  *ptr_opfile = opfile;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  if ( cum_opfile != NULL ) { 
    unlink(cum_opfile);
    free_if_non_null(cum_opfile);
  }
  free_if_non_null(ivals);
  rs_munmap(cum_X, cum_nX);
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  return(status);
}
