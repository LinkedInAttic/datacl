#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "bin_search.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_join.h"
#include "bin_search.h"
#include "get_nR.h"
#include "aux_fld_meta.h"
#include "mk_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "is_nn_fld.h"
#include "chunking.h"
#include "meta_globals.h"

/* This is used when f2 is sorted ascending but f1 is not */
/* Also assume that f2 has no null values */
/* Implementation uses binary search which I do not like */
/* Usually used when T1 is large and T2 is small */

int g_thread_id[MAX_NUM_THREADS];
long long g_num_rows_processed[MAX_NUM_THREADS];
/* g_num_rows_processed is to make sure that exactly nR rows processed*/
static int g_nT; /* num_threads */
long long  g_nR1;
long long  g_nR2;
char *g_f1_X;
char *g_nn_f1_X;
char *g_f2_X;
char *g_nn_f2_X;
char *g_src_f2_X;
char *g_dst_f1_X;
int g_ifldtype;
bool g_b_write_dst;
char *g_cfX;

void *core_is_a_in_b(
		     void *arg
		     )
{
  int status = 0;
  int iminval, imaxval;
  long llminval, llmaxval;
  int i1val, prev_i1val, isrc_val;
  long long ll1val, prev_ll1val, llsrc_val;

  int tid = *((int *)arg);
  // fprintf(stderr, "Hello World! It's me, thread # [%d]!\n", tid);
  /* De-reference global variables */
  int *if1  = (int *)g_f1_X; 
  long long *llf1 = (long long *)g_f1_X;
  int *if2  = (int *)g_f2_X; 
  long long *llf2 = (long long *)g_f2_X;
  int *isrc_f2 = (int *)g_src_f2_X; 
  long long *llsrc_f2 = (long long *)g_src_f2_X;
  int *idst_f1 = (int *)g_dst_f1_X; 
  long long *lldst_f1 = (long long *)g_dst_f1_X;
  long long nR1 = g_nR1;
  long long nR2 = g_nR2;
  int nT = g_nT;
  char *nn_f1_X = g_nn_f1_X;
  int ifldtype = g_ifldtype;
  bool b_write_dst = g_b_write_dst;
  char *cfX = g_cfX;

  /* We find the smallest and largest values of f2 which allow us to
   * skip the binary search over the values of f2 */
  switch ( ifldtype ) { 
  case FLDTYPE_INT : 
    iminval = if2[0];
    imaxval = if2[nR2-1];
    break;
  case FLDTYPE_LONGLONG : 
    llminval = llf2[0];
    llmaxval = llf2[nR2-1];
    break;
  default : 
    go_BYE(-1);
    break;
  }
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
    /*------------------------------------------------------------------*/
    bool prev_defined = false; 
    /* prev_defined tells us whetrher prev_i1val or prev_ll1val are defined */
    long long prev_pos = -1; // undefined value 
    for ( long long i1 = lb; i1 < ub; i1++ ) {
      bool is_found;
      long long pos;
      is_found = false;
      /* If f1 is null, then cfld and dst_f1 must also be null */
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
	isrc_val = 0;
	cfX[i1] = FALSE;
	if ( b_write_dst ) { 
	  lldst_f1[i1] = 0;
	}
	continue;
      }
      /*----------------------------------------------------------------*/
      switch ( ifldtype ) { 
      case FLDTYPE_INT : 
	i1val = if1[i1];
	if ( ( i1val < iminval ) || ( i1val > imaxval ) ) {
	  pos = -1;
	}
	else {
	  if ( ( prev_defined ) && ( i1val == prev_i1val ) ) { 
	    if ( prev_pos >= 0 ) { 
	      pos = prev_pos;
	    }
	    else {
	      status = bin_search_i(if2, nR2, i1val, &pos, "");
	      cBYE(status);
	    }
	  }
	  else {
	    status = bin_search_i(if2, nR2, i1val, &pos, "");
	    cBYE(status);
	  }
	}
	if ( pos >= 0 ) { is_found = true; }
	if ( b_write_dst ) { 
	  if ( pos >= 0 ) { 
	    isrc_val = isrc_f2[pos];
	  }
	  else {
	    isrc_val = 0;
	  }
	  idst_f1[i1] = isrc_val;
	}
	break;
      case FLDTYPE_LONGLONG : 
	ll1val = llf1[i1];
	if ( ( ll1val < llminval ) || ( ll1val > llmaxval ) ) {
	  pos = -1;
	}
	else {
	  if ( ( prev_defined ) && ( ll1val == prev_ll1val ) ) { 
	    if ( prev_pos >= 0 ) { 
	      pos = prev_pos;
	    }
	    else {
	      status = bin_search_ll(llf2, nR2, ll1val, &pos, "");
	      cBYE(status);
	    }
	  }
	  else {
	    status = bin_search_ll(llf2, nR2, ll1val, &pos, "");
	    cBYE(status);
	  }
	}
	if ( pos >= 0 ) { is_found = true; } 
	if ( b_write_dst ) { 
	  if ( pos >= 0 ) { 
	    llsrc_val = llsrc_f2[pos];
	  }
	  else {
	    llsrc_val = 0;
	  }
	  lldst_f1[i1] = llsrc_val;
	}
	break;
      default :
	go_BYE(-1);
	break;
      }
      prev_i1val = i1val;
      prev_ll1val = ll1val;
      prev_defined = true;
      if ( is_found ) { 
	cfX[i1] = TRUE;
      }
      else {
	cfX[i1] = FALSE;
      }
    }
  }
BYE:
  if ( status == 0 ) {
    return ((void *)0);
  }
  else {
    return ((void *)1);
  }
}

//---------------------------------------------------------------
// START FUNC DECL
int 
is_a_in_b(
	    char *t1,
	    char *f1,
	    char *t2,
	    char *f2,
	    char *cfld,
	    char *src_f2, /* fld_to_fetch_from_t2 */
	    char *dst_f1 /* fld_to_place_in_t1 */
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *src_f2_X = NULL; size_t src_f2_nX = 0;
  char *dst_f1_X = NULL; size_t dst_f1_nX = 0;
  char *cfX = NULL; size_t cf_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_src_f2_X = NULL; size_t nn_src_f2_nX = 0;
  FLD_TYPE *f1_meta = NULL, *f2_meta = NULL, *src_f2_meta = NULL;
  FLD_TYPE *nn_f1_meta = NULL, *nn_src_f2_meta = NULL;
  long long nR1, nR2, chk_nR1 = 0;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *dst_ofp = NULL; char *dst_opfile = NULL;
  char str_meta_data[4096];
  int t1_id, t2_id, f1_id, nn_f1_id, f2_id, nn_f2_id, src_f2_id, nn_src_f2_id;
  int dst_f1_id, nn_dst_f1_id, cfld_id;
  int ifldtype, src_f2_ifldtype;
  bool b_write_dst;
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
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) {  
    /* condition field NOT provided */
    if ( ( src_f2 == NULL ) || ( *src_f2 == '\0' ) ) { go_BYE(-1); }
    if ( ( dst_f1 == NULL ) || ( *dst_f1 == '\0' ) ) { go_BYE(-1); }
    if ( strcmp(f1, dst_f1) == 0 ) { go_BYE(-1); }
  }
  else {
    if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
    if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { go_BYE(-1); }
    if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  zero_string(str_meta_data, 4096);
  //--------------------------------------------------------

  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;
  status = is_fld(NULL, t1_id, f1, &f1_id);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0);
  cBYE(status);

  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbl[t2_id].nR;
  status = is_fld(NULL, t2_id, f2, &f2_id);
  chk_range(f2_id, 0, g_n_fld);
  f2_meta = &(g_fld[f2_id]);
  status = rs_mmap(f2_meta->filename, &f2_X, &f2_nX, 0); cBYE(status);

  //--------------------------------------------------------
  status = mk_ifldtype(f1_meta->fldtype, &ifldtype);
  cBYE(status);
  if ( strcmp(f1_meta->fldtype, f2_meta->fldtype) != 0 ) {
    fprintf(stderr, "Fields being compared must be same type \n");
    go_BYE(-1); 
  } 
  //--------------------------------------------------------
  if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { 
    status = is_fld(NULL, t2_id, src_f2, &src_f2_id);
    src_f2_meta = &(g_fld[src_f2_id]);
    status = rs_mmap(src_f2_meta->filename, &src_f2_X, &src_f2_nX, 0); 
    cBYE(status);
    nn_src_f2_id = g_fld[src_f2_id].nn_fld_id;
    if ( nn_src_f2_id >= 0 ) { 
      nn_src_f2_meta = &(g_fld[nn_src_f2_id]);
      status=rs_mmap(nn_src_f2_meta->filename, &nn_src_f2_X, &nn_src_f2_nX, 0); 
      cBYE(status);
    }
    status = mk_ifldtype(src_f2_meta->fldtype, &src_f2_ifldtype);
    cBYE(status);
  }
  /*------------------------------------------------------------*/
  // There can be null values in f1 but not in f2
  nn_f1_id = f1_meta->nn_fld_id;
  if ( nn_f1_id >= 0 ) { 
    chk_range(nn_f1_id, 0, g_n_fld);
    nn_f1_meta = &(g_fld[nn_f1_id]);
    status = rs_mmap(nn_f1_meta->filename, &nn_f1_X, &nn_f1_nX, 0); 
    cBYE(status);
  }
  nn_f2_id = f2_meta->nn_fld_id;
  if ( nn_f2_id >= 0 ) { go_BYE(-1); }
  /* Determine whether a destination value needs to be written in T1 */
  b_write_dst = false;
  if ( ( dst_f1 != NULL ) && ( *dst_f1 != '\0' ) ) {
    b_write_dst = true;
    if ( strcmp(f1_meta->fldtype, src_f2_meta->fldtype) != 0 ) {
      fprintf(stderr, "Field being transferred must have same type as \
	  that being compared \n");
      go_BYE(-1); 
    }
  }
  /*------------------------------------------------------------*/
  /* Make space for output condition field */
  long long filesz = nR1 * sizeof(char);
  status = open_temp_file(&ofp, &opfile, filesz); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(opfile, filesz); cBYE(status);
  status = rs_mmap(opfile, &cfX, &cf_nX, 1); cBYE(status);
  /* Make space for output value field */
  if ( b_write_dst ) { 
    long long filesz = nR1 * src_f2_meta->n_sizeof;
    status = open_temp_file(&dst_ofp, &dst_opfile, filesz); cBYE(status);
    fclose_if_non_null(dst_ofp);
    status = mk_file(dst_opfile, filesz); cBYE(status);
    status = rs_mmap(dst_opfile, &dst_f1_X, &dst_f1_nX, 1); // writing 
  }
  //--------------------------------------------------------
  /* Set up arguments for function call as global variables */
  g_f1_X = f1_X;
  g_f2_X = f2_X;
  g_src_f2_X = src_f2_X;
  g_dst_f1_X = dst_f1_X;
  g_nR1 = nR1;
  g_nR2 = nR2;
  g_ifldtype = ifldtype;
  g_b_write_dst = b_write_dst;
  g_cfX = cfX;

  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    g_thread_id[i] = i;
    g_num_rows_processed[i] = 0;
  }
  status = get_num_threads(&g_nT);
  cBYE(status);
  //--------------------------------------------
#define MIN_ROWS_FOR_IS_A_IN_B 65536
  if ( nR1 <= MIN_ROWS_FOR_IS_A_IN_B ) {
    g_nT = 1;
  }
  /* Don't create more threads than you can use */
  if ( g_nT > nR1 ) { g_nT = nR1; }

  if ( g_nT == 1 ) { 
    core_is_a_in_b(&(g_thread_id[0]));
    chk_nR1 = g_num_rows_processed[0];
  }
  else { /* Create threads */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_nT; t++ ) { 
      rc = pthread_create(&threads[t], NULL, core_is_a_in_b, &(g_thread_id[t]));
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



  // Add output field to meta data 
  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    sprintf(str_meta_data, "n_sizeof=1:fldtype=bool:filename=%s", opfile);
    status = add_fld(t1, cfld, str_meta_data, &cfld_id);
    cBYE(status);
  }
  else {
    switch ( src_f2_ifldtype ) { 
    case FLDTYPE_INT : 
      sprintf(str_meta_data, "n_sizeof=%lu:fldtype=int:filename=%s", 
	      sizeof(int), dst_opfile);
      break;
    case FLDTYPE_LONGLONG : 
      sprintf(str_meta_data, "n_sizeof=%lu:fldtype=int:filename=%s", 
	      sizeof(long long), dst_opfile);
      break;
    }
    status = add_fld(t1, dst_f1, str_meta_data, &dst_f1_id);
    cBYE(status);
    /* TODO: We need to add nn field only if necessary */
    status = add_aux_fld(t1, dst_f1, opfile, "nn", &nn_dst_f1_id);
    cBYE(status);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(src_f2_X, src_f2_nX);
  rs_munmap(dst_f1_X, dst_f1_nX);
  rs_munmap(cfX, cf_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(nn_src_f2_X, nn_src_f2_nX);
  free_if_non_null(opfile);
  fclose_if_non_null(ofp);
  free_if_non_null(dst_opfile);
  fclose_if_non_null(dst_ofp);
  return(status);
}
