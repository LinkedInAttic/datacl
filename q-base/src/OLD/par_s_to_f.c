#include <pthread.h>
#include "qtypes.h"
#include "mmap.h"
#include "./AUTOGEN/GENFILES/extract_I2.h"
#include "./AUTOGEN/GENFILES/extract_I4.h"
#include "./AUTOGEN/GENFILES/extract_I8.h"
#include "./AUTOGEN/GENFILES/s_to_f_const_I2.h"
#include "./AUTOGEN/GENFILES/s_to_f_const_I4.h"
#include "./AUTOGEN/GENFILES/s_to_f_const_I8.h"
#include "./AUTOGEN/GENFILES/s_to_f_seq_I2.h"
#include "./AUTOGEN/GENFILES/s_to_f_seq_I4.h"
#include "./AUTOGEN/GENFILES/s_to_f_seq_I8.h"
 #include "extract_S.h"

extern int g_block_size;
extern int g_num_threads;

static char *sg_X;
static const char *sg_str_scalar;
static long long sg_lb;
static long long sg_ub;
static int sg_thread_id[MAX_NUM_THREADS];
static int sg_num_threads_used;

void *core_s_to_f(
    void *arg
    )
{
  int status = 0;
  int tid = *((int *)arg);
  int val4, start4, incr4;
  long long val8, start8, incr8;
  short val2, start2, incr2;
#define MAX_LEN 32
  char op[MAX_LEN];
  char fldtype[MAX_LEN];
  bool is_null = true;

  zero_string(op, MAX_LEN);
  zero_string(fldtype, MAX_LEN);

  status = extract_S(sg_str_scalar, "op=[", "]", op, MAX_LEN, &is_null); 
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  status = extract_S(sg_str_scalar, "fldtype=[", "]", fldtype, MAX_LEN, 
      &is_null); 
  cBYE(status);
  if ( is_null ) { go_BYE(-1); }
  if ( ( tid < 0 ) || ( tid >= MAX_NUM_THREADS ) ) { go_BYE(-1); }
  if ( strcmp(op, "const") == 0 ) {
    if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(sg_str_scalar, "val=[", "]", &val2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = s_to_f_const_I2((short *)sg_X, sg_lb, sg_ub, val2); 
      cBYE(status);
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(sg_str_scalar, "val=[", "]", &val4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = s_to_f_const_I4((int *)sg_X, sg_lb, sg_ub, val4); 
      cBYE(status);
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(sg_str_scalar, "val=[", "]", &val8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }

      status = s_to_f_const_I8((long long *)sg_X, sg_lb, sg_ub, val8); 
      cBYE(status);
    }
  }
  else if ( strcmp(op, "seq") == 0 ) {
    if ( strcmp(fldtype, "I2") == 0 ) {
      status = extract_I2(sg_str_scalar, "start=[", "]", &start2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I2(sg_str_scalar, "incr=[", "]",  &incr2, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = s_to_f_seq_I2((short *)sg_X, sg_lb, sg_ub, start2, incr2); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
    }
    else if ( strcmp(fldtype, "I4") == 0 ) {
      status = extract_I4(sg_str_scalar, "start=[", "]", &start4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I4(sg_str_scalar, "incr=[", "]",  &incr4, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = s_to_f_seq_I4((int *)sg_X, sg_lb, sg_ub, start4, incr4); 
      cBYE(status);
    }
    else if ( strcmp(fldtype, "I8") == 0 ) {
      status = extract_I8(sg_str_scalar, "start=[", "]", &start8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = extract_I8(sg_str_scalar, "incr=[", "]",  &incr8, &is_null); 
      cBYE(status);
      if ( is_null ) { go_BYE(status); }
      status = s_to_f_seq_I8((long long *)sg_X, sg_lb, sg_ub, start8, incr8); 
      cBYE(status);
    }
  }
  else {
    go_BYE(-1); 
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

// START FUNC DECL
int
par_s_to_f(
    char *X,
    long long lb,
    long long ub,
    const char *str_scalar
    )
// STOP FUNC DECL
{
  int status = 0;
  int rc;
  pthread_t threads[MAX_NUM_THREADS];
  pthread_attr_t attr;

  // Set up for thread access to data 
  sg_X = X;
  sg_str_scalar = str_scalar;
  sg_lb = lb;
  sg_ub = ub;
  for ( int i = 0; i < MAX_NUM_THREADS; i++ ) { sg_thread_id[i] = i; }

  if ( ( ub - lb) > ( g_block_size * g_num_threads ) ) {
    /* go parallel */
    sg_num_threads_used = g_num_threads;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for ( int t = 0; t < g_num_threads; t++ ) { 
      rc = pthread_create(&threads[t], &attr, core_s_to_f, &(sg_thread_id[t]));
      if ( rc ) { go_BYE(-1); }
    }
  }
  else { /* go sequential */
    sg_num_threads_used = 1;
    core_s_to_f(&sg_thread_id[0]); }
BYE:
  return(status);
}
