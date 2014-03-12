#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "sqlite3.h"
#include "fld_meta.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "mk_file.h"
#include "chunking.h"

void *is_a_in_b_core(
		    void *threadid
		    )
{
  int status = 0;
  int nT;

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

// START FUNC DECL
int
par_is_in_b(
    char *f1_X, 
    FLD_META_TYPE f1_meta, 
    long long nR1,
    char *f2_X, 
    FLD_META_TYPE f2_meta, 
    long long nR2,
    char *src_f2_X, 
    char *nn_src_f2_X, 
    FLD_META_TYPE src_f2_meta, 
    char **ptr_opfile,
    char **ptr_dst_opfile,
    char **ptr_nn_dst_opfile
    )
// STOP FUNC DECL
{
  int status = 0;
  char *opfile = NULL, *dst_opfile = NULL, *nn_dst_opfile = NULL;
  char *op_X = NULL; size_t op_nX = 0;
  char *dst_X = NULL; size_t dst_nX = 0;
  char *nn_dst_X = NULL; size_t nn_dst_nX = 0;
  FILE *ofp = NULL; int nT;

  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status = NULL;

  // Make file for output condition field
  op_nX = nR1 * sizeof(char);
  status = open_temp_file(&ofp, &opfile);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, op_nX);
  cBYE(status);
  status = rs_mmap(opfile, &op_X, &op_nX, 1); // open for reading
  cBYE(status);
  // Make file for dst field if src field provided
  if ( src_f2_X != NULL ) { 
    dst_nX = nR1 * src_f2_meta.n_sizeof;
    status = open_temp_file(&ofp, &dst_opfile);
    fclose_if_non_null(ofp);
    status = mk_file(dst_opfile, dst_nX);
    cBYE(status);
    status = rs_mmap(dst_opfile, &dst_X, &dst_nX, 1); // open for reading
    cBYE(status);
    // Make nn field for src field. May delete this later
    op_nX = nR1 * sizeof(char);
    status = open_temp_file(&ofp, &opfile);
    fclose_if_non_null(ofp);
    status = mk_file(opfile, op_nX);
    cBYE(status);
    status = rs_mmap(opfile, &op_X, &op_nX, 1); // open for reading
    cBYE(status);
  }


  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
  if ( ( nR1 <= MIN_ROWS_FOR_PARALLELISM ) || ( nT == 1 ) ) { 
    nT = 1;
    thread_status = is_a_in_b_core(0);
  }
  else {
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < nT; t++ ) { 
      // fprintf(stderr, "In main: creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, is_a_in_b_core, (void *)&t);
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
BYE:
  rs_munmap(op_X, op_nX);
  rs_munmap(dst_X, dst_nX);
  rs_munmap(nn_dst_X, nn_dst_nX);
  return(status);
}
