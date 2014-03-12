#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "f1opf2_cum.h"
#include "f1opf2_shift.h"
#include "f1opf2_hash.h"
#include "vec_f1opf2.h"
#include "aux_fld_meta.h"
#include "mk_file.h"
#include "s_to_f.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "meta_globals.h"
#include "subsample.h"
#include "chunking.h"

extern bool g_write_to_temp_dir;

static int g_thread_id[MAX_NUM_THREADS];
static int g_nT; /* num_threads */
static int  *g_f1ptr;
static int  *g_f2ptr;
static char *g_usedptr;
static long long g_nR1;
static long long g_nR2;


void *core_subsample(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  /* "De-reference global variables */
  int nT = g_nT;
  int *f1ptr = g_f1ptr;
  int *f2ptr = g_f2ptr;
  char *usedptr = g_usedptr;
  long long nR1 = g_nR1;
  long long nR2 = g_nR2;
  long long n1, n2, b1, b2, lb1, ub1, lb2, ub2;


  b1 = nR1 / nT;
  b2 = nR2 / nT;
  //------------------------------------------------
  if ( ( tid < 0 ) || ( tid > nT ) ) { go_BYE(-1); }
  for ( int t = 0; t < nT; t++ ) {
    if ( t != tid ) { continue; }
    lb1 = ( b1 * t ); ub1 = lb1 + b1;
    if ( t == (nT - 1 ) ) { ub1 = nR1; }

    lb2 = ( b2 * t ); ub2 = lb2 + b2;
    if ( t == (nT - 1 ) ) { ub2 = nR2; }

    int *iptr1 = f1ptr + lb1;
    n1 = ub1 - lb1;
    int *iptr2 = f2ptr + lb2;
    n2 = ub2 - lb2;
    char *used = usedptr + lb1;
    status = subsample_int(iptr1, n1, iptr2, n2, used);
    cBYE(status);
  }
BYE:
  if ( status == 0 ) {
    return ((void *)0);
  }
  else {
    return ((void *)1); }
}
//---------------------------------------------------------------
// START FUNC DECL
int 
subsample(
       char *t1,
       char *f1,
       char *str_ratio,
       char *t2,
       char *f2
       )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *f_used_X = NULL; size_t f_used_nX = 0;
  FLD_TYPE *f1_meta = NULL, *f_used_meta = NULL; 
  long long nR1, nR2; 
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN, f_used_id = INT_MIN;
  char str_meta_data[1024];
  int ratio;
  FILE *ofp = NULL; char *opfile = NULL;
  // For multi-threading 
  int rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  char *endptr;
  ratio = strtoll(str_ratio, &endptr, 10);
  if ( ratio < 2 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;
  status = is_fld(NULL, t1_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); 
  cBYE(status);
  // Not implemented for following cases 
  if ( g_fld[f1_id].nn_fld_id >= 0 ) { go_BYE(-1); }
  if ( strcmp(f1_meta->fldtype, "int") != 0 ) { go_BYE(-1); }
  if ( nR1 <= 2 ) { go_BYE(-1); }
  //---------------------------------------------
  // Allocate space
  nR2 = nR1 / ratio;
  if ( nR2 < 1 ) { go_BYE(-1); }
  long long filesz = f1_meta->n_sizeof * nR2;
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &f2_X, &f2_nX, 1); 

  //--- Decide on how much parallelism to use
  int nT;
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
  }
  status = get_num_threads(&nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_SUBSAMPLE 10000 // 1048576
  if ( nR2 <= MIN_ROWS_FOR_SUBSAMPLE ) {
    nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( nT > nR2 ) { nT = nR2; }
  //--------------------------------------------
  //---------------------------------------------
  char f_used[MAX_LEN_FLD_NAME];
  zero_string(f_used, MAX_LEN_FLD_NAME);
  status = qd_uq_str(f_used, MAX_LEN_FILE_NAME); cBYE(status);
  status = s_to_f(t1, f_used, "op=const:val=false:fldtype=bool");
  cBYE(status);
  status = is_fld(NULL, t1_id, f_used, &f_used_id); cBYE(status);
  chk_range(f_used_id, 0, g_n_fld);
  f_used_meta = &(g_fld[f_used_id]);
  status = rs_mmap(f_used_meta->filename, &f_used_X, &f_used_nX, 1); 
  cBYE(status);

  int *f1ptr = NULL, *f2ptr = NULL; char *usedptr = NULL;
  f1ptr = (int *)f1_X;
  f2ptr = (int *)f2_X;
  usedptr = (char *)f_used_X;

  // Set up global variables
  g_nT = nT;
  g_f1ptr = f1ptr;
  g_f2ptr = f2ptr;
  g_usedptr = usedptr;
  g_nR1 = nR1;
  g_nR2 = nR2;
  if ( g_nT == 1 ) { 
    core_subsample(&(g_thread_id[0]));
  }
  else {
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_subsample,
	  &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
    }
  }
  /* SEQUENTIAL CODE 
  status  = subsample_int((int *)f1_X, nR1, (int *)f2_X, nR2, f_used_X);
  */
  status = del_fld(NULL, t1_id, f_used, -1); cBYE(status);

  //--- Add to t2
  status  = del_tbl(t2, -1); cBYE(status);
  char str_nR2[32];
  sprintf(str_nR2, "%lld", nR2);
  status = add_tbl(t2, str_nR2, &t2_id); cBYE(status);
  sprintf(str_meta_data, "fldtype=int:n_sizeof=4:filename=%s", opfile);
  status = add_fld(t2, f2, str_meta_data, &f2_id); cBYE(status);

BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f_used_X, f_used_nX);
  fclose_if_non_null(ofp);
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  free_if_non_null(opfile);
  return(status);
}

// START FUNC DECL 
int
subsample_int(
	  int *X,
	  long long nX,
	  int *Y,
	  long long nY,
	  char *used
	  )
// STOP FUNC DECL 
{
  int status = 0;
  long long nYprime =0;
  if ( X == NULL ) { go_BYE(-1); }
  if ( Y == NULL ) { go_BYE(-1); }
  if ( nX <= 0 ) { go_BYE(-1); }
  if ( nY <= 0 ) { go_BYE(-1); }
  if ( nY >= nX ) { go_BYE(-1); }
  double frac = ((double)nY / (double)nX);
  double ratio = 1 / frac;

  // This is for cheap version of rng
  long long i, bmask;
  for ( i = 1; ; i *= 2 ) {
    if ( i > ratio ) { break; }
  }
  bmask = i -1; 
  //--------------------------------
#undef GOOD_RNG


  srand48(2059879141); // TODO Consider improving

  int chk_val = 1;
  for ( ; nYprime < nY; ) { 
    for ( long long i = 0; i < nX; i++ ) { 
      bool good = false;
#ifdef GOOD_RNG
      if ( drand48() <= frac ) {
	good = true;
      }
#else
      if ( ( i & bmask ) == chk_val ) {
	good = true;
      }
#endif
      if ( good ) { 
	if ( used[i] == FALSE ) {
	  used[i] = TRUE;
	  Y[nYprime++] = X[i];
	  if ( nYprime == nY ) { break; }
	}
      }
    }
    chk_val++; // Important!
  }
 BYE:
  return(status);
}

#undef UNIT_TEST
#ifdef UNIT_TEST
#define NX 100
#define NY 10
int
main()
{
  int status = 0;
  int X[NX];
  int Y[NY];
  char used[NX];

  for ( int i = 0; i < NX; i++ ) { 
    X[i] = i+1;
    used[i] = FALSE;
  }
  for ( int i = 0; i < NY; i++ ) { 
    Y[i] = 0;
  }
  status = core_subsample_int(X, NX, Y, NY, used);
  cBYE(status);
  for ( int i = 0; i < NY; i++ ) { 
    printf("%3d:%5d\n", i, Y[i]);
  }
BYE:
  return(status);
}
#endif
