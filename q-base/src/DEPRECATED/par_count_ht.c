#include <pthread.h>
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"
#include "set_meta.h"
#include "aux_meta.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "add_fld.h"
#include "count_ht.h"
#include "mk_file.h"
#include "open_temp_file.h"
#include "sort2_asc_longlong.h"
#include "mix.h"

typedef struct _HT_I8_TYPE { 
  unsigned long long key;
  unsigned long long count;
} HT_I8_TYPE;

extern long long g_block_size;
extern int g_num_threads;

HT_I8_TYPE *sg_hts[MAX_NUM_THREADS];
int sg_sz_ht;    // size of hash table 
int sg_n_hts[MAX_NUM_THREADS];  // number of occupied entries in hash table 
long long sg_max_count;

pthread_mutex_t sg_mutexsum;
int sg_thread_id[MAX_NUM_THREADS];
long long sg_num_probes[MAX_NUM_THREADS];
int sg_num_threads_used;
char *sg_X;
char *sg_nn_X;
FLD_TYPE sg_fldtype;
long long sg_src_nR;


void *
core_count_ht(
	      void *arg
	      )
{
  int status = 0;
  int tid = *((int *)arg);
  char *I1ptr = NULL; char I1val;
  short *I2ptr = NULL; short I2val;
  int *I4ptr = NULL; int I4val;
  long long *I8ptr = NULL; long long I8val;
  int *ptr_occupancy; 

  unsigned int ht_loc;
  unsigned long long ulval;
  int pos_ht;
  bool found, exists;
  long long sg_max_count = 0;
  HT_I8_TYPE *lht = NULL; 

  I1ptr = (char *)sg_X;
  I2ptr = (short *)sg_X;
  I4ptr = (int *)sg_X;
  I8ptr = (long long *)sg_X;

  ptr_occupancy = sg_n_hts + tid;
  long long block_size = sg_src_nR / sg_num_threads_used;
  long long lb = block_size * tid;
  long long ub = lb + block_size;
  if ( tid == ( sg_num_threads_used-1) ) {
    ub = sg_src_nR;
  }
  lht = sg_hts[tid]; // local hash table 
  fprintf(stderr, "Thread %d processing [%lld, %lld] \n", tid, lb, ub);
  for ( long long i = lb; i < ub; i++ ) {
    if ( ( sg_nn_X != NULL ) && ( sg_nn_X[i] == FALSE ) ) {
      continue;
    }
    switch ( sg_fldtype ) { 
    case I4 : 
      I4val = I4ptr[i]; ulval = (unsigned long long)I4val;
      break;
    case I8 : 
      I8val = I8ptr[i]; ulval = (unsigned long long)I8val;
      break;
    default : 
      go_BYE(-1);
      break;
    }
    ht_loc = mix_I8(ulval) % sg_sz_ht;
    pos_ht = -1;
    // Determine whether you have seen this guy before or not
    // If not, then you also need to find the position to put him
    found = false; 
    int num_probes = 0;
    for ( int i = ht_loc; i < sg_sz_ht; i++ ) { 
      num_probes++;
      if ( lht[i].key == ulval ) { 
	pos_ht = i;
	exists = true;
	found = true;
	break;
      }
      if ( lht[i].key == 0 ) { // unoccupied 
	pos_ht = i;
	found = true;
	exists = false;
	break;
      }
    }
    if ( found == false ) {
      for ( int i = 0; i < ht_loc; i++ ) { 
        num_probes++;
	if ( lht[i].key == ulval ) { 
	  pos_ht = i;
	  exists = true;
	  found = true;
	  break;
	}
	if ( lht[i].key == 0 ) { // unoccupied 
	  pos_ht = i;
	  found = true;
	  exists = false;
	  break;
	}
      }
    }
    sg_num_probes[tid] += num_probes;
    if ( found == false ) { go_BYE(-1); }
    if ( pos_ht < 0 ) { go_BYE(-1); }
    if ( exists ) {
      lht[pos_ht].count++;
      /*
      if ( lht[pos_ht].count > sg_max_count ) { 
	sg_max_count = lht[pos_ht].count;
      }
      */
    }
    else {
      lht[pos_ht].count = 1;
      lht[pos_ht].key = ulval;
      *ptr_occupancy = *ptr_occupancy + 1;
    }
    if ( *ptr_occupancy > ( sg_sz_ht >> 1 ) ) {
      fprintf(stderr, "TO BE IMPLEMENTED\n");
      go_BYE(-1);
    }
  }
 BYE:
  if ( sg_num_threads_used == 1 ) {
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
count_ht(
	 char *src_tbl,
	 char *fld,
	 char *dst_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *Z = NULL;    size_t nZ = 0;
  char *X = NULL;    size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  TBL_REC_TYPE src_tbl_rec; int src_tbl_id; 
  TBL_REC_TYPE dst_tbl_rec; int dst_tbl_id; 
  FLD_REC_TYPE fld_rec;     int fld_id; 
  FLD_REC_TYPE nn_fld_rec;  int nn_fld_id; 
  FLD_REC_TYPE val_fld_rec;  int val_fld_id; 
  FLD_REC_TYPE cnt_fld_rec;  int cnt_fld_id; 
  long long src_nR, dst_nR;
#define MAX_LEN 32
  char op[MAX_LEN];
  char str_dst_nR[MAX_LEN];
  char valfile[MAX_LEN_FILE_NAME+1];
  char cntfile[MAX_LEN_FILE_NAME+1];
  char *val_X = NULL; size_t n_val_X = 0;
  char *cnt_X = NULL; size_t n_cnt_X = 0;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd;
  // for final dumping of results
  int *I4ptr = NULL; int I4val; 
  long long *I8ptr = NULL; long long I8val;
  //-- for pthreads
  int rc;
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;
  void *thread_status;
  /* STRANGE! Parallel code is slower than sequential code */
  bool is_parallel_ok = true; 
  HT_I8_TYPE *ht = NULL;  int n_ht = 0;
  char tempfile[MAX_LEN_FILE_NAME+1]; FILE *tfp = NULL;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  zero_string(op, MAX_LEN);
  zero_string(str_dst_nR, MAX_LEN);
  zero_string(valfile, MAX_LEN_FILE_NAME+1);
  zero_string(cntfile, MAX_LEN_FILE_NAME+1);
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  src_nR = g_tbls[src_tbl_id].nR;
  status = is_fld(NULL, src_tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);

  mcr_cd;
  status = rs_mmap(fld_rec.filename, &X, &nX, 0); cBYE(status);
  mcr_uncd;
  if ( nn_fld_id >= 0 ) { 
    status = rs_mmap(nn_fld_rec.filename, &nn_X, &nn_nX, 0); cBYE(status);
    cBYE(status);
  }
  if ( src_nR >= INT_MAX ) { go_BYE(-1); }

  sg_sz_ht = prime_geq(65536);

  sg_max_count = 0;

  // Set up for thread access to data 
  sg_X = X;
  sg_nn_X = nn_X;
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { sg_thread_id[i] = i; }
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { sg_num_probes[i] = i; }
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { sg_hts[i] = NULL; }
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { sg_n_hts[i] = 0; }
  sg_fldtype = fld_rec.fldtype;
  sg_src_nR = src_nR;
  // -----------------------------------------------------
  if ( ( is_parallel_ok ) && ( src_nR > ( g_block_size * g_num_threads ) ) 
      && ( g_num_threads > 1 ) ) {
    /* go parallel */
    sg_num_threads_used = g_num_threads;
    // allocate individual hash tables 
    for ( int j = 0; j < sg_num_threads_used; j++ ) { 
      sg_hts[j] = (HT_I8_TYPE *)malloc(sg_sz_ht * sizeof(HT_I8_TYPE));
      return_if_malloc_failed(sg_hts[j]);
      for ( int i = 0; i < sg_sz_ht; i++ ) { 
        sg_hts[j][i].key = 0;
        sg_hts[j][i].count = 0;
      }
    }
    //-----------------------------------------
    pthread_mutex_init(&sg_mutexsum, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_num_threads; t++ ) { 
      rc = pthread_create(&threads[t], &attr, core_count_ht, 
			  &(sg_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for ( int t = 0; t < g_num_threads; t++ ) {
      rc = pthread_join(threads[t], &thread_status);
      if ( rc ) { go_BYE(-1); }
    }
    pthread_mutex_destroy(&sg_mutexsum);
  }
  else { /* go sequential */
    sg_num_threads_used = 1;
    for ( int j = 0; j < sg_num_threads_used; j++ ) { 
      sg_hts[j] = (HT_I8_TYPE *)malloc(sg_sz_ht * sizeof(HT_I8_TYPE));
      return_if_malloc_failed(sg_hts[j]);
      for ( int i = 0; i < sg_sz_ht; i++ ) { 
        sg_hts[j][i].key = 0;
        sg_hts[j][i].count = 0;
      }
    }
    core_count_ht(&sg_thread_id[0]); 
  }
  //-------------------------------------------------------
  // Combine several hash tables into one called ht
  if ( sg_num_threads_used == 1 ) {
    n_ht = sg_n_hts[0];
    ht = (HT_I8_TYPE *)malloc(n_ht * sizeof(HT_I8_TYPE));
    return_if_malloc_failed(ht);
    int ctr = 0;
    for ( int i = 0; i < sg_sz_ht; i++ ) { 
      if ( sg_hts[0][i].count == 0 ) { continue; }
      if ( ctr >= n_ht ) { go_BYE(-1); }
      ht[ctr] = sg_hts[0][i];
      ctr++;
    }
    if ( ctr != n_ht ) { go_BYE(-1); }
  }
  else {
    status = open_temp_file(tempfile, 0); cBYE(status);
    mcr_cd;
    tfp = fopen(tempfile, "wb");
    return_if_fopen_failed(tfp, tempfile, "wb");
    HT_I8_TYPE *tht = NULL; int n_tht = 0;
    for ( int i = 0; i < sg_num_threads_used; i++ ) { 
      tht = sg_hts[i];
      for ( int j = 0; j < sg_sz_ht; j++ ) {
	if ( tht[j].count == 0 ) { continue; }
	fwrite(tht+j, sizeof(HT_I8_TYPE), 1, tfp);
	n_tht++;
      }
    }
    fclose_if_non_null(tfp);
    fprintf(stderr, "Created %s \n", tempfile);
    status = rs_mmap(tempfile, &Z, &nZ, 1); cBYE(status);
    mcr_uncd;
    qsort(Z, n_tht, sizeof(HT_I8_TYPE), sort2_asc_longlong);
    rs_munmap(Z, nZ);
    fprintf(stderr, "TO BE IMPLEMENTED\n");
      go_BYE(-1);
  }

  dst_nR = n_ht;
  sprintf(str_dst_nR, "%lld", dst_nR);
  //-------------------------------------------------------
  status = del_tbl(dst_tbl, -1); cBYE(status);
  status = add_tbl(dst_tbl, str_dst_nR, &dst_tbl_id, &dst_tbl_rec); 
  cBYE(status);
  int fldsz; status = get_fld_sz(fld_rec.fldtype, &fldsz); cBYE(status);

  mcr_cd;
  status = mk_temp_file(valfile, fldsz * dst_nR); cBYE(status);
  status = rs_mmap(valfile, &val_X, &n_val_X, 1); cBYE(status);
  mcr_uncd;
  I4ptr = (int *)val_X;
  I8ptr = (long long *)val_X;
  for ( int i = 0; i < n_ht; i++ ) {
    if ( ht[i].count == 0 ) { go_BYE(-1); }
    switch ( fld_rec.fldtype ) { 
    case I4 : I4val = ht[i].key; *I4ptr++ = I4val; break;
    case I8 : I8val = ht[i].key; *I8ptr++ = I8val; break;
    default : go_BYE(-1); break;
    }
  }
  rs_munmap(val_X, n_val_X);
  status = add_fld(dst_tbl_id, "value", valfile, fld_rec.fldtype, 
		   &val_fld_id, &val_fld_rec);

  if ( sg_max_count >= INT_MAX ) { fprintf(stderr, "TODO\n"); go_BYE(-1); }

  // TODO!!! SWITCH COUNT FROM LONG LOGNG TO INT 
  mcr_cd;
  // status = mk_temp_file(cntfile, sizeof(int) * dst_nR); cBYE(status);
  status = mk_temp_file(cntfile, sizeof(long long) * dst_nR); cBYE(status);
  status = rs_mmap(cntfile, &cnt_X, &n_cnt_X, 1); cBYE(status);
  mcr_uncd;
  I4ptr = (int *)cnt_X;
  I8ptr = (long long *)cnt_X;
  for ( int i = 0; i < n_ht; i++ ) {
    if ( ht[i].count == 0 ) { go_BYE(-1); }
    // I4val = ht[i].count; *I4ptr++ = I4val; 
    I8val = ht[i].count; *I8ptr++ = I8val; 
  }
  rs_munmap(cnt_X, n_cnt_X);
  // status = add_fld(dst_tbl_id, "count", cntfile, I4, &cnt_fld_id, &cnt_fld_rec);
  status = add_fld(dst_tbl_id, "count", cntfile, I8, &cnt_fld_id, &cnt_fld_rec);
  cBYE(status);
  long long total_sg_num_probes = 0;
  for ( int i = 0; i < sg_num_threads_used; i++ ) { 
    total_sg_num_probes += sg_num_probes[i];
  }
  fprintf(stderr, "Average num probes = %lf \n", 
      (double)total_sg_num_probes/(double)src_nR);

 BYE:
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { 
    free_if_non_null(sg_hts[i]);
  }
  free_if_non_null(ht);
  rs_munmap(Z, nZ);
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  rs_munmap(val_X, n_val_X);
  rs_munmap(cnt_X, n_cnt_X);
  return(status);
}
