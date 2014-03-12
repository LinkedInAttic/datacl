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
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "open_temp_file.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "del_fld.h"
#include "chunking.h"
#include "f_to_s.h"
#include "meta_globals.h"

#include "core_srt_join_I_I_I.h"
#include "core_srt_join_I_I_L.h"
#include "core_srt_join_I_L_I.h"
#include "core_srt_join_I_L_L.h"
#include "core_srt_join_L_I_I.h"
#include "core_srt_join_L_I_L.h"
#include "core_srt_join_L_L_I.h"
#include "core_srt_join_L_L_L.h"


int g_thread_worked[MAX_NUM_THREADS];
int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS];
/* g_num_rows_processed is to make sure that exactly nR rows processed */
static int g_nT; /* num_threads */
char *g_src_lnk_X = NULL;
char *g_src_val_X = NULL; 
long long g_src_nR = LLONG_MIN;
char *g_dst_lnk_X = NULL; 
char *g_dst_val_X = NULL; 
char *g_nn_dst_val_X = NULL;
long long g_dst_nR = LLONG_MIN;
int  g_ijoin_op = -1; 
bool g_is_any_null;
FLD_TYPE *g_src_lnk_meta = NULL;
FLD_TYPE *g_src_val_meta = NULL;
FLD_TYPE *g_dst_lnk_meta = NULL;
/* srt_join is used when the source link fields and destination fields are
 * sorted and have no null values */
//---------------------------------------------------------------
void *core_srt_join(
		    void *arg
		    )
{
  int status = 0;
  int *iptr;
  iptr = (int *)arg;
  int tid = (int)(*iptr);
  // printf("Hello World! It's me, thread # [%d]!\n", tid);
  if ( ( tid < 0 ) || ( tid > g_nT ) ) { go_BYE(-1); }
  g_thread_worked[tid] = TRUE;
  // De-reference global variables
  char *src_lnk_X = g_src_lnk_X;
  char *src_val_X =  g_src_val_X; 
  long long src_nR =  g_src_nR;
  char *dst_lnk_X =  g_dst_lnk_X; 
  char *dst_val_X =  g_dst_val_X; 
  char *nn_dst_val_X =  g_nn_dst_val_X;
  long long dst_nR = g_dst_nR;
  int  ijoin_op = g_ijoin_op; 
  FLD_TYPE *src_lnk_meta = g_src_lnk_meta;
  FLD_TYPE *src_val_meta = g_src_val_meta;
  FLD_TYPE *dst_lnk_meta = g_dst_lnk_meta;
  bool is_any_null = false;
  //------------------------------------------------
  for ( int t = 0; t < g_nT; t++ ) {
    if ( t != tid ) { continue; }
    long long block_size = dst_nR / g_nT;
    long long dst_lb = t * block_size;
    long long dst_ub = dst_lb + block_size;
    if ( t == ( g_nT-1 ) ) { /* last thread picks up slack */
      dst_ub = dst_nR;
    }
    g_num_rows_processed[t] = (dst_ub - dst_lb);
    /*
      fprintf(stderr, "Thread %d processing [%d, %d ] \n", tid, 
      (long long) dst_lb, (long long)dst_ub);
    */
    // Core join 
    if ( ( strcmp(src_lnk_meta->fldtype, "int") == 0 ) && 
	 ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						"int") ) == 0 ) && 
	 ( strcmp(dst_lnk_meta->fldtype, "int") == 0 ) ) {
      status = core_srt_join_I_I_I(
				   (int *)src_lnk_X, (int *)src_val_X, src_nR,
				   (int *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "int") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "int") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "long long") == 0 ) ) {
      status = core_srt_join_I_I_L(
				   (int *)src_lnk_X, (int *)src_val_X, src_nR,
				   (long long *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "int") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "long long") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "int") == 0 ) ) {
      status = core_srt_join_I_L_I(
				   (int *)src_lnk_X, (long long
						      *)src_val_X, src_nR,
				   (int *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "int") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "long long") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "long long") == 0 ) ) {
      status = core_srt_join_I_L_L((int *)src_lnk_X, 
				   (long long *)src_val_X, src_nR, (long long *)dst_lnk_X, 
				   (long long *)dst_val_X, nn_dst_val_X, dst_lb, dst_ub, ijoin_op, 
				   &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "long long") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "int") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "int") == 0 ) ) {
      status = core_srt_join_L_I_I(
				   (long long *)src_lnk_X, (int
							    *)src_val_X, src_nR,
				   (int *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "long long") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "int") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "long long") == 0 ) ) {
      status = core_srt_join_L_I_L(
				   (long long *)src_lnk_X, (int
							    *)src_val_X, src_nR,
				   (long long *)dst_lnk_X, (int *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "long long") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "long long") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "int") == 0 ) ) {
      status = core_srt_join_L_L_I(
				   (long long *)src_lnk_X, (long long
							    *)src_val_X, src_nR,
				   (int *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else if ( ( strcmp(src_lnk_meta->fldtype, "long long") == 0 ) && 
	      ( ( src_val_meta == NULL ) || ( strcmp(src_val_meta->fldtype,
						     "long long") ) == 0 ) && 
	      ( strcmp(dst_lnk_meta->fldtype, "long long") == 0 ) ) {
      status = core_srt_join_L_L_L(
				   (long long *)src_lnk_X, (long long
							    *)src_val_X, src_nR,
				   (long long *)dst_lnk_X, (long long *)dst_val_X, nn_dst_val_X, 
				   dst_lb, dst_ub, ijoin_op, &is_any_null);
    }
    else { go_BYE(-1); }
  }
  if ( is_any_null ) { g_is_any_null = true; }
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


// START FUNC DECL
int 
srt_join(
	 char *src_tbl,
	 char *src_lnk,
	 char *src_val,
	 char *dst_tbl,
	 char *dst_lnk,
	 char *dst_fld,
	 char *op
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *src_val_X = NULL; size_t src_val_nX = 0;
  char *src_lnk_X = NULL; size_t src_lnk_nX = 0;
  int dst_n_sizeof = INT_MIN; char dst_fldtype[MAX_LEN_FLD_TYPE+1];

  char *nn_dst_val_X = NULL; size_t nn_dst_val_nX = 0;
  char *dst_val_X = NULL; size_t dst_val_nX = 0;
  char *dst_lnk_X = NULL; size_t dst_lnk_nX = 0;
  FLD_TYPE *src_val_meta = NULL;
  FLD_TYPE *src_lnk_meta = NULL;
  FLD_TYPE *dst_lnk_meta = NULL;
  long long src_nR = INT_MIN, dst_nR = INT_MIN, chk_dst_nR = 0;
  char str_meta_data[1024]; 
  int ijoin_op; bool is_any_null = false;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_lnk_id = INT_MIN, dst_lnk_id = INT_MIN; 
  int src_val_id = INT_MIN;
  int dst_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;
  FILE *ofp = NULL, *nn_ofp = NULL;
  char *opfile = NULL, *nn_opfile = NULL;
  // For multi-threading 
  int   rc; // result code for thread create 
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status; char str_rslt[32];
  FLD_TYPE temp_fld_meta; // used when src_val == NULL 
  //----------------------------------------------------------------
  zero_string(str_rslt, 32);
  zero_string(str_meta_data, 1024);
  zero_fld_meta(&temp_fld_meta);
  zero_string(dst_fldtype, MAX_LEN_FLD_TYPE+1);
  status = mk_mjoin_op(op, &ijoin_op);
  cBYE(status);
  //----------------------------------------------------------------
  // Get meta-data for all necessary fields 
  status = is_tbl(src_tbl, &src_tbl_id); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbl[src_tbl_id].nR;

  status = is_tbl(dst_tbl, &dst_tbl_id); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  dst_nR = g_tbl[dst_tbl_id].nR;

  status = is_fld(NULL, src_tbl_id, src_lnk, &src_lnk_id); cBYE(status);
  chk_range(src_lnk_id, 0, g_n_fld);
  src_lnk_meta = &(g_fld[src_lnk_id]);

  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = is_fld(NULL, src_tbl_id, src_val, &src_val_id); cBYE(status);
    chk_range(src_val_id, 0, g_n_fld);
    src_val_meta = &(g_fld[src_val_id]);
  }


  status = is_fld(NULL, dst_tbl_id, dst_lnk, &dst_lnk_id); cBYE(status);
  chk_range(dst_lnk_id, 0, g_n_fld);
  dst_lnk_meta = &(g_fld[dst_lnk_id]);
  //----------------------------------------------------------------
  /* Make sure link fields are sorted ascending */
  status = f_to_s(src_tbl, src_lnk, "is_sorted", str_rslt);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n",
	    src_lnk, src_tbl);
    go_BYE(-1);
  }
  status = f_to_s(dst_tbl, dst_lnk, "is_sorted", str_rslt);
  cBYE(status);
  if ( strcmp(str_rslt, "ascending") != 0 ) {
    fprintf(stderr, "Field [%s] in Table [%s] not sorted ascending\n", 
	    dst_lnk, dst_tbl);
    go_BYE(-1);
  }
  //----------------------------------------------------------------
  // Get pointer access to all necessary fields
  status = rs_mmap(src_lnk_meta->filename, &src_lnk_X, &src_lnk_nX, 0); 
  cBYE(status);
  if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) { 
    status = rs_mmap(src_val_meta->filename, &src_val_X, &src_val_nX, 0); 
    cBYE(status);
  }
  status = rs_mmap(dst_lnk_meta->filename, &dst_lnk_X, &dst_lnk_nX, 0); 
  cBYE(status);

  //--------------------------------------------------------
  // Create output data files
  if ( ( ( src_val != NULL ) && ( *src_val != '\0' ) ) || 
    ( strcmp(op, "cnt") == 0 ) ) {
    /* we need to make an output file */
    if ( ( src_val != NULL ) && ( *src_val != '\0' ) ) {
      if ( strcmp(op, "cnt") == 0 ) { go_BYE(-1); }
      dst_n_sizeof = src_val_meta->n_sizeof;
      strcpy(dst_fldtype, src_val_meta->fldtype);
    }
    else {
      if ( src_nR < INT_MAX ) { 
	dst_n_sizeof = sizeof(int);
	strcpy(dst_fldtype, "int");
	strcpy(temp_fld_meta.fldtype, "int");
      }
      else {
	dst_n_sizeof = sizeof(long long);
	strcpy(dst_fldtype, "long long");
	strcpy(temp_fld_meta.fldtype, "long long");
      }
    }
    long long filesz = dst_n_sizeof * dst_nR;
    status = open_temp_file(&ofp, &opfile, filesz); cBYE(status); 
    fclose_if_non_null(ofp);
    status = mk_file(opfile, filesz); cBYE(status);
    status = rs_mmap(opfile, &dst_val_X, &dst_val_nX, 1); cBYE(status);
    if ( src_val_meta == NULL ) { 
      strcpy(temp_fld_meta.fldtype, "int");
      src_val_meta = &temp_fld_meta;
    }
  }
  long long nnfilesz = sizeof(char) * dst_nR;
  status = open_temp_file(&nn_ofp, &nn_opfile, nnfilesz); cBYE(status); 
  fclose_if_non_null(nn_ofp);
  status = mk_file(nn_opfile, nnfilesz); cBYE(status);
  status = rs_mmap(nn_opfile, &nn_dst_val_X, &nn_dst_val_nX, 1); cBYE(status);
  //--------------------------------------------
  // Set up values needed by core into globals
  g_src_lnk_X = src_lnk_X;
  g_src_val_X = src_val_X ;
  g_src_nR = src_nR;
  g_dst_lnk_X = dst_lnk_X;
  g_dst_val_X = dst_val_X;
  g_nn_dst_val_X = nn_dst_val_X;
  g_dst_nR = dst_nR;
  g_ijoin_op = ijoin_op; 
  g_is_any_null = false;
  g_src_lnk_meta = src_lnk_meta;
  g_src_val_meta = src_val_meta;
  g_dst_lnk_meta = dst_lnk_meta;
  //--------------------------------------------

  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_worked[i] = FALSE;
    g_thread_id[i] = i;
    g_num_rows_processed[i] = 0;

  }
#define MIN_ROWS_FOR_PARALLEL_JOIN 32
  status = get_num_threads(&g_nT);
  cBYE(status);
  if ( g_nT > 1 ) { 
    if ( dst_nR <= MIN_ROWS_FOR_PARALLEL_JOIN ) {
      g_nT = 1;
    }
  }
  /* Don't create more threads than you can use */
  if ( g_nT > dst_nR ) { g_nT = dst_nR; }

  if ( g_nT == 1 ) { 
    core_srt_join(&(g_thread_id[0]));
  }
  else { /* Create threads */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_srt_join, &(g_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
    }
  }
  is_any_null = g_is_any_null;
  for ( int t = 0; t < g_nT; t++ ) { 
    if ( g_thread_worked[t] == FALSE ) { 
      fprintf(stderr, "ERROR! Thread %d did no work \n", t); go_BYE(-1); 
    }
    chk_dst_nR += g_num_rows_processed[t];
  }
  if ( chk_dst_nR != dst_nR ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Add output field to meta data 
  if ( ( ( src_val != NULL ) && ( *src_val != '\0' ) ) ||
       ( strcmp(op, "cnt") == 0 ) ) {
    sprintf(str_meta_data,"filename=%s:n_sizeof=%d:fldtype=%s", opfile,
	    dst_n_sizeof, dst_fldtype);
    status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id);
    cBYE(status);
    if ( is_any_null == true ) {
      status = add_aux_fld(dst_tbl, dst_fld, nn_opfile, "nn", &nn_dst_fld_id);
      cBYE(status);
    }
    else {
      unlink(nn_opfile);
      free_if_non_null(nn_opfile);
    }
  }
  else {
    sprintf(str_meta_data,"filename=%s:n_sizeof=%ld:fldtype=bool", 
	    nn_opfile, sizeof(char));
    status = add_fld(dst_tbl, dst_fld, str_meta_data, &dst_fld_id);
    cBYE(status);
  }
 BYE:
  rs_munmap(src_val_X, src_val_nX);
  rs_munmap(src_lnk_X, src_lnk_nX);
  rs_munmap(dst_val_X, dst_val_nX);
  rs_munmap(nn_dst_val_X, nn_dst_val_nX);
  rs_munmap(dst_lnk_X, dst_lnk_nX);
  free_if_non_null(nn_opfile);
  free_if_non_null(opfile);
  return(status);
}
