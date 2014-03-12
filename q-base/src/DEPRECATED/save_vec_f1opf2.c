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
#include "vec_f1opf2.h"
#include "get_sz_type_op_fld.h"
#include "mk_file.h"
#include "chunking.h"
#include "assign_char.h"

#include "conv_double_to_int.h"
#include "conv_double_to_longlong.h"
#include "conv_double_to_float.h"

#include "conv_int_to_bool.h"
#include "conv_int_to_longlong.h"
#include "conv_int_to_float.h"

#include "conv_longlong_to_int.h"
#include "conv_longlong_to_bool.h"
#include "conv_longlong_to_float.h"

#include "conv_bool_to_int.h"
#include "conv_bool_to_longlong.h"

#include "conv_float_to_int.h"
#include "conv_float_to_longlong.h"
#include "conv_float_to_double.h"

#include "ones_complement_int.h"
#include "not_int.h"
#include "ones_complement_longlong.h"
#include "not_longlong.h"
#include "ones_complement_bool.h"
#include "not_bool.h"

#include "incr_int.h"
#include "decr_int.h"
#include "incr_longlong.h"
#include "decr_longlong.h"


#include "bitcount_int.h"
#include "bitcount_longlong.h"

#include "vec_sqrt.h"

int g_thread_worked[MAX_NUM_THREADS];
int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS]; 
/* g_num_rows_processed is to make sure that exactly nR rows processed */
static int g_nT; /* num_threads */
static long long g_nR; /* number of rows to process */
static int   g_f1type;
static char *g_f1_X;
static char *g_nn_f1_X;
static char *g_sz_f1_X;
static char *g_op_X; /* where to write output */
static char *g_op;   /* operation to be performed */
static char *dst_fld_type = NULL;
static char *date_format = NULL;

void *f1opf2_core(
		    void *arg
		    )
{
  int status = 0;
  char *g1_X, *nn_g1_X; 
  long long lb, lb1, lb2, ub, ub1, ub2;

  int num_rows_this_thread; /* number of rows for this thread to process */
  long long nB; /* number of blocks to process in inner loop */
  long long nX; /* number of rows handed to vector code */
  int nC = CACHE_SIZE; /* number of rows to process at a time (cache aware) */

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


  /*-------------------------------------*/
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    status = get_outer_lb_ub(nR, nT, t, &lb1, &ub1);
    cBYE(status);
    num_rows_this_thread = ub1 - lb1;
    /* nR divisible by nT => some thread may have no work */
    if ( num_rows_this_thread <= 0 ) { continue; }
    status = get_num_blocks( num_rows_this_thread, nC, &nB);
    if ( status < 0 ) { 
      printf("hi\n");
    }
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
      fprintf(stderr, " Thread %d working on %lld rows, [%lld, %lld] \n", 
	  t, nX, lb, ub);
	  */
      nn_g1_X = (nn_f1_X + ( sizeof(char) * lb ));
      if ( f1type == FLDTYPE_DOUBLE ) {
        g1_X = (f1_X + ( sizeof(double) * lb ));
	if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "float") == 0 ) ) {
	  my_op_X = (op_X + ( sizeof(float) * lb ));
	  conv_double_to_float((double *)g1_X, nX, (float *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "long long") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  conv_double_to_longlong((double *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "int") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  conv_double_to_int((double *)g1_X, nX, (int *)my_op_X);
	}
      }
      else if ( f1type == FLDTYPE_INT ) {
        g1_X = (f1_X + ( sizeof(int) * lb ));
	if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "bool") == 0 ) ) {
	  my_op_X = (op_X + ( sizeof(char) * lb ));
	  conv_int_to_bool((int *)g1_X, nX, (char *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "long long") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  conv_int_to_longlong((int *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "float") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(float) * lb ));
	  conv_int_to_float((int *)g1_X, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, "!") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  not_int((int *)g1_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "~") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  ones_complement_int((int *)g1_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "++") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  incr_int((int *)g1_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "--") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  decr_int((int *)g1_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "bitcount") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bitcount_int((int *)g1_X, nX, (int *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if (f1type == FLDTYPE_LONGLONG ) {
        g1_X = (f1_X + ( sizeof(long long) * lb ));
	if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "bool") == 0 ) ) {
	  my_op_X = (op_X + ( sizeof(char) * lb ));
	  conv_longlong_to_bool((long long *)g1_X, nX, (char *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "int") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  conv_longlong_to_int((long long *)g1_X, nX, (int *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "float") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(float) * lb ));
	  conv_longlong_to_float((long long *)g1_X, nX, (float *)my_op_X);
	}
	else if ( strcmp(op, "!") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  not_longlong((long long *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "~") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  ones_complement_longlong((long long *)g1_X, nX, 
	      (long long *)my_op_X);
	}
	else if ( strcmp(op, "++") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  incr_longlong((long long *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "--") == 0 ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  decr_longlong((long long *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( strcmp(op, "bitcount") == 0 ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  bitcount_longlong((long long *)g1_X, nX, (int *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_FLOAT ) {
        g1_X = (f1_X + ( sizeof(float) * lb ));
	if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "int") == 0 ) ) {
	  my_op_X = (op_X + ( sizeof(int) * lb ));
	  conv_float_to_int((float *)g1_X, nX, (int *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "long long") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(long long) * lb ));
	  conv_float_to_longlong((float *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "double") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(double) * lb ));
	  conv_float_to_double((float *)g1_X, nX, (double *)my_op_X);
	}
	else if ( strcmp(op, "sqrt") == 0 ) {
	  my_op_X = (op_X + ( sizeof(float) * lb ));
	  vec_sqrt((float *)g1_X, nX, (float *)my_op_X);
	}
	else { go_BYE(-1); }
      }
      else if ( f1type == FLDTYPE_BOOL ) {
        g1_X = (f1_X + ( sizeof(bool) * lb ));
	if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "bool") == 0 ) ) {
	  my_op_X = (op_X + ( sizeof(long long) * lb ));
	  conv_bool_to_longlong((char *)g1_X, nX, (long long *)my_op_X);
	}
	else if ( ( strcmp(op, "conv") == 0 ) && ( dst_fld_type != NULL ) && 
	    ( strcmp(dst_fld_type, "int") == 0 ) ) {
          my_op_X = (op_X + ( sizeof(int) * lb ));
	  conv_bool_to_int((char *)g1_X, nX, (int *)my_op_X);
	}
	else if ( strcmp(op, "!") == 0 ) {
          my_op_X = (op_X + ( sizeof(bool) * lb ));
	  not_bool((char *)g1_X, nX, (char *)my_op_X);
	}
	else if ( strcmp(op, "~") == 0 ) {
          my_op_X = (op_X + ( sizeof(bool) * lb ));
	  ones_complement_bool((char *)g1_X, nX, (char *)my_op_X);
	}
	else { go_BYE(-1); }
      }
    }
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
vec_f1opf2(
	     long long in_nR,
	     int in_f1type,
	     char *str_meta_data,
	     char *in_f1_X,
	     char *in_nn_f1_X,
	     char *in_sz_f1_X,
	     char *in_op,
	     char *in_str_spec,
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
  FLD_TYPE out_fld_meta; int optype;
  char *op_X = NULL; size_t n_op_X = 0 ; /* for mmaped output file */
  char *nn_op_X = NULL; size_t n_nn_op_X = 0 ; /* for mmaped nn file */
  char *op = in_op;
  int f1type = in_f1type;
  long long chk_nR = 0;
  long long nR = in_nR;

  
  /* Get destination field type if op = conv */
  if ( strcmp(op, "conv") == 0 ) {
    status = extract_name_value(in_str_spec, "newtype=", ":", &dst_fld_type);
    cBYE(status);
    if ( ( dst_fld_type == NULL ) || ( *dst_fld_type == '\0' ) ) {
      go_BYE(-1); 
    }
  }
  if ( strcmp(op, "dateconv") == 0 ) {
    status = extract_name_value(in_str_spec, "format=", ":", &date_format);
    cBYE(status);
    if ( ( date_format == NULL ) || ( *date_format == '\0' ) ) {
      go_BYE(-1); 
    }
  }
  //--------------------------------------------
  // create an output file and mmap it 
  status = open_temp_file(&ofp, &opfile); cBYE(status);
  fclose_if_non_null(ofp);
  status = get_sz_type_op_fld("f1opf2", op, f1type, -1, dst_fld_type, 
      &optype, &sizeof_op);
  cBYE(status);
  //------------------------------------------
  zero_fld_meta(&out_fld_meta);
  strcpy(out_fld_meta.filename, opfile);
  status = mk_str_fldtype(optype, out_fld_meta.fldtype);
  cBYE(status);
  out_fld_meta.n_sizeof = sizeof_op;
  if ( strcmp(in_op, "conv") == 0 ) { 
    sprintf(str_meta_data, "filename=%s:fldtype=%s:n_sizeof=%d",
	opfile, dst_fld_type, sizeof_op);
  }
  else {
    status = fld_meta_to_str(out_fld_meta, str_meta_data);
    cBYE(status);
  }
  //------------------------------------------
  n_op_X = sizeof_op * nR;
  status = mk_file(opfile, n_op_X); cBYE(status);
  status = rs_mmap(opfile, &op_X, &n_op_X, 1); cBYE(status);
  //--------------------------------------------
  if ( in_nn_f1_X == NULL ) {
    /* No need for nn file */
  }
  else {
    status = open_temp_file(&nn_ofp, &nn_opfile);
    cBYE(status);
    fclose_if_non_null(nn_ofp);
    status = mk_file(nn_opfile, nR);
    cBYE(status);
    status = rs_mmap(nn_opfile, &nn_op_X, &n_nn_op_X, 1);
    cBYE(status);
    assign_char(nn_op_X, in_nn_f1_X, nR);
    cBYE(status);
  }
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
  status = get_num_threads(&g_nT);
  cBYE(status);
  // fprintf(stderr, "Parallelism set to %d \n", g_nT);
#define MIN_ROWS_FOR_PARALLELISM_F1OPF2 4 // 65536
  if ( nR <= MIN_ROWS_FOR_PARALLELISM_F1OPF2 ) {
    g_nT = 1;
    // fprintf(stderr, "NUm rows (%lld) too few \n", nR);
  }
  /* Don't create more threads than you can use */
  if ( g_nT > nR ) { g_nT = nR; }
  if ( g_nT == 1 ) { 
    void *vstat;
    vstat = f1opf2_core(&(g_thread_id[0]));
    chk_nR = g_num_rows_processed[0];
  }
  else {
    // Create threads
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, f1opf2_core,
	  &(g_thread_id[t]));
      if ( rc ) {
	fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
	go_BYE(-1);
      }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    chk_nR = 0;
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_nR += g_num_rows_processed[t];
      /*
      fprintf(stderr, " Thread %d worked on %lld rows, total = %lld \n",
	  t, g_num_rows_processed[t], chk_nR);
	  */
    }
  }
  if ( chk_nR != in_nR ) { go_BYE(-1); }
  //--------------------------------------------
  *ptr_opfile = opfile;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  rs_munmap(op_X, n_op_X);
  rs_munmap(nn_op_X, n_nn_op_X);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  free_if_non_null(dst_fld_type);
  free_if_non_null(date_format);
  return(status);
}
