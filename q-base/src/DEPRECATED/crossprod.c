#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "f_to_s.h"
#include "mk_file.h"
#include "copy_fld.h"
#include "chunking.h"
#include "meta_globals.h"

#include "core_crossprod_I_I.h"
#include "core_crossprod_I_L.h"
#include "core_crossprod_L_I.h"
#include "core_crossprod_L_L.h"

int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS];
/* g_num_rows_processed is to make sure that exactly nR rows processed*/
static int g_nT; /* num_threads */
long long  g_nR1;
long long  g_nR2;
long long  g_nR3;
int g_f1type;
int g_f2type;
long long g_f1size;
long long g_f2size;
char *g_f1_X;
char *g_f2_X;
char *g_Y1;
char *g_Y2;
//---------------------------------------------------------------
void *core_crossprod(
		     void *arg
		     )
{
  int status = 0;
  int tid = *((int *)arg);
  // fprintf(stderr, "Hello World! It's me, thread # [%d]!\n", tid);
  //------------------------------------------------
  /* "De-reference global variables */
  int nT = g_nT;
  long long nR1 = g_nR1;
  long long nR2 = g_nR2;
  long long nR3 = g_nR3;
  int f1type = g_f1type;
  int f2type = g_f2type;
  long long  f1size = g_f1size;
  long long  f2size = g_f2size;
  char *f1_X = g_f1_X;
  char *f2_X = g_f2_X;
  char *Y1 = g_Y1;
  char *Y2 = g_Y2;
  //------------------------------------------------
  if ( ( tid < 0 ) || ( tid > nT ) ) { go_BYE(-1); }
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    /* Now we divvy up work among the threads. We split the rows of
     * field 1 over the threads */
    long long block_size = nR1 / nT;
    long long lb = t * block_size;
    long long ub = lb + block_size;
    if ( t == ( nT-1 ) ) { /* last thread picks up slack */
      ub = nR1;
    }
    g_num_rows_processed[t] = (ub - lb); /* for debugging */
    // fprintf(stderr, "Thread # [%d] doing %lld work\n", t, (ub -lb));
    /* Do some pointer offsetting so that each thread is doing the right
     * thing */
    f1_X += (f1size * lb); /* Bump this up by lb rows */
    f2_X = f2_X; /* This stays the same */
    Y1 += (f2size * nR2 * lb); 
    Y2 += (f2size * nR2 * lb); 

    /* Core join starts below */


    //----------------------------------------------------------------
    if ( ( f1type == FLDTYPE_INT ) && ( f2type == FLDTYPE_INT ) ) { 
      status = core_crossprod_I_I(f1_X, (ub-lb), f2_X, nR2, Y1, Y2,
	  (ub-lb)*nR2);
    }
    else if ((f1type == FLDTYPE_INT)&&(f2type == FLDTYPE_LONGLONG)) { 
      status = core_crossprod_I_L(f1_X, nR1, f2_X, nR2, Y1, Y2, nR3);
    }
    else if ((f1type == FLDTYPE_LONGLONG)&&(f2type == FLDTYPE_INT)) { 
      status = core_crossprod_L_I(f1_X, nR1, f2_X, nR2, Y1, Y2, nR3);
    }
    else if ((f1type == FLDTYPE_LONGLONG)&&(f2type == FLDTYPE_LONGLONG)) { 
      status = core_crossprod_L_L(f1_X, nR1, f2_X, nR2, Y1, Y2, nR3);
    }
    else { go_BYE(-1); }
    cBYE(status);
  }
 BYE:
  if ( status == 0 ) {
    return ((void *)0);
  }
  else {
    return ((void *)1);
  }

}

static int get_data(
		    char *tbl, 
		    char *fld,
		    long long *ptr_nR,
		    char **ptr_X,
		    size_t *ptr_nX,
		    char **ptr_opfile,
		    int *ptr_fldtype,
		    int *ptr_fldsz
		    )
{
  int status = 0;
  int tbl_id = INT_MIN, fld_id = INT_MIN, nn_fld_id = INT_MIN;
  char *X = NULL;    size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *op_X = NULL; size_t op_nX = 0;
  char *endptr = NULL;
  char buffer[32];
  long long nR = 0;
  FLD_TYPE *fld_meta = NULL, *nn_fld_meta = NULL;
  FILE *ofp = NULL; char *opfile = NULL;

  zero_string(buffer, 32);

  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id); cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  nR = g_tbl[tbl_id].nR;
  fld_meta = &(g_fld[fld_id]);
  nn_fld_id = g_fld[fld_id].nn_fld_id;
  if ( nn_fld_id >= 0 ) { 
    nn_fld_meta = &(g_fld[nn_fld_id]);
    status = rs_mmap(nn_fld_meta->filename, &nn_X, &nn_nX, 0);
    cBYE(status);
  }
  *ptr_fldsz = fld_meta->n_sizeof;
  status = mk_ifldtype(fld_meta->fldtype, ptr_fldtype); cBYE(status);

  switch ( *ptr_fldtype ) { 
  case FLDTYPE_INT : 
  case FLDTYPE_LONGLONG : 
    /* all is well */
    break;
  default : 
    go_BYE(-1);
  }
  if ( nn_X == NULL ) {  /* no nn field */
    status = rs_mmap(fld_meta->filename, &X, &nX, 0);
    cBYE(status);
    *ptr_nR = nR;
    *ptr_X  = X;
    *ptr_nX = nX;
    *ptr_opfile = NULL;
  }
  else {
    long long nn_nR = 0;
    status = f_to_s(tbl, nn_fld_meta->name, "sum", buffer);
    cBYE(status);
    nn_nR = strtol(buffer, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( ( nn_nR <= 0 ) || ( nn_nR >= nR ) ) { go_BYE(-1); }
    /* Now make a copy of just the non-null values */
    status = rs_mmap(fld_meta->filename, &X, &nX, 0);
    cBYE(status);
    status = rs_mmap(nn_fld_meta->filename, &nn_X, &nn_nX, 0);
    cBYE(status);
    long long filesz = nn_nR * fld_meta->n_sizeof;
    status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(opfile, filesz); cBYE(status);
    status = rs_mmap(opfile, &op_X, &op_nX, 1); cBYE(status);
    status = copy_nn_vals(X, nR, nn_X, op_X, nn_nR, fld_meta->n_sizeof);
    cBYE(status);
    /* Cleanup and return values */
    rs_munmap(X, nX);
    rs_munmap(nn_X, nn_nX);
    *ptr_nR = nn_nR;
    *ptr_X  = op_X;
    *ptr_nX = op_nX;
    *ptr_opfile = opfile;
  }
 BYE:
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int 
crossprod(
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *t3
	  )
// STOP FUNC DECL
{
  int status = 0;

  char *Y1 = NULL; size_t nY1 = 0;
  char *Y2 = NULL; size_t nY2 = 0;

  char *f1_X = NULL; size_t f1_nX = 0; char *f1_opfile = NULL; 
  int f1type, f2type;
  char *f2_X = NULL; size_t f2_nX = 0; char *f2_opfile = NULL; 
  long long f1size, f2size;

  int t2f1_fld_id = INT_MIN, t2f2_fld_id = INT_MIN;
  int t3_id = INT_MIN, itemp; 
  long long chk_nR1 = 0, nR1, nR2, nR3;
  char str_meta_data[1024];
  char *t3f1_opfile = NULL, *t3f2_opfile = NULL;
  FILE *ofp = NULL;
  char buffer[32];
  // For multi-threading 
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  zero_string(buffer, 32);
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); } 
  /* Remove f1 != f2 restriction later. To do so, we need to specify
   * fields of t3 explicitly */
  //----------------------------------------------------------------
  status = get_data(t1, f1, &nR1, &f1_X, &f1_nX, &f1_opfile, &f1type, &f1size);
  cBYE(status);
  status = get_data(t2, f2, &nR2, &f2_X, &f2_nX, &f2_opfile,  &f2type, &f2size);
  cBYE(status);
  nR3 = nR1 * nR2;
  if ( nR3 == 0 ) {
    fprintf(stderr, "No data to create t3 \n");
    goto BYE;
  }
  // Create storage for field 1 in Table t3 */
  long long  filesz = nR3 * f1size;
  status = open_temp_file(&ofp, &t3f1_opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(t3f1_opfile, filesz); cBYE(status);
  status = rs_mmap(t3f1_opfile, &Y1, &nY1, 1); cBYE(status);
  // Create storage for field 2 in Table t3 */
  filesz = nR3 * f2size;
  status = open_temp_file(&ofp, &t3f2_opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(t3f2_opfile, filesz); cBYE(status);
  status = rs_mmap(t3f2_opfile, &Y2, &nY2, 1); cBYE(status);
  //----------------------------------------------------------------
  /* Set up parallelism computations. Parallelization strategy is
   * simple. Partition field 1 (nR1 rows) among the threads */
  g_nR1 = nR1;
  g_nR2 = nR2;
  g_nR3 = nR3;
  g_f1type = f1type;
  g_f2type = f2type;
  g_f1size = f1size;
  g_f2size = f2size;
  g_f1_X = f1_X;
  g_f2_X = f2_X;
  g_Y1 = Y1;
  g_Y2 = Y2;
 
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
    g_num_rows_processed[i] = 0;
  }
  status = get_num_threads(&g_nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_CROSSPROD 4 // 1024
  if ( nR1 <= MIN_ROWS_FOR_CROSSPROD ) {
    g_nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( g_nT > nR1 ) { g_nT = nR1; }

  if ( g_nT == 1 ) { 
    core_crossprod(&(g_thread_id[0]));
    chk_nR1 = g_num_rows_processed[0];
  }
  else { /* Create threads */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_crossprod,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
      chk_nR1 += g_num_rows_processed[t];
    }
  }
  if ( chk_nR1 != nR1 ) { go_BYE(-1); }
  //----------------------------------------------------------------
  // Add output fields to t3 meta data 
  status = is_tbl(t3, &t3_id); cBYE(status);
  if ( t3_id >= 0 ) { 
    status = del_tbl(NULL, t3_id);
    cBYE(status);
  }
  sprintf(buffer, "%lld", nR3);
  status = add_tbl(t3, buffer, &itemp); cBYE(status);

  sprintf(str_meta_data, "fldtype=int:n_sizeof=%u:filename=%s",
	  f1size, t3f1_opfile);
  status = add_fld(t3, f1, str_meta_data, &t2f1_fld_id); cBYE(status);
  zero_string(str_meta_data, 1024);
  sprintf(str_meta_data, "fldtype=int:n_sizeof=%u:filename=%s", 
	  f2size, t3f2_opfile);
  status = add_fld(t3, f2, str_meta_data, &t2f2_fld_id); cBYE(status);
 BYE:
  fclose_if_non_null(ofp);
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  if ( f1_opfile != NULL ) { 
    unlink(f1_opfile); free_if_non_null(f1_opfile);
  }
  if ( f2_opfile != NULL ) { 
    unlink(f2_opfile); free_if_non_null(f2_opfile);
  }
  free_if_non_null(t3f1_opfile);
  free_if_non_null(t3f2_opfile);
  return(status);
}
// START FUNC DECL
int
copy_nn_vals(
	     char *X_in,
	     long long n_in,
	     char *nn_X_in,
	     char *X_out,
	     long long n_out,
	     int fld_sz
	     )
// STOP FUNC DECL
{
  int status = 0;
  long long idx = 0;
  for ( long long i = 0; i < n_in; i++ ) { 
    if ( nn_X_in[i] == TRUE ) { 
      memcpy(X_out, X_in, fld_sz);
      X_out += fld_sz;
      idx++;
    }
    X_in += fld_sz;
  }
  if ( idx != n_out ) { go_BYE(-1); }
 BYE:
  return(status);
}
