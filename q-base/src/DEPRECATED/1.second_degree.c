/*
CC=icc
FLAGS="-O3 -Wall -std=c99"
LINKFLAGS=" -lm -ipp -DIPP"
#---
CC=gcc
FLAGS="-O4 -Wall -std=gnu99"
LINKFLAGS=" -lm "
#---
$CC ${FLAGS} -I./AUTOGEN/GENFILES -I. \
	second_degree.c \
	auxil.c \
	mmap.c \
        ./AUTOGEN/GENFILES/assign_I1.c \
	./AUTOGEN/GENFILES/qsort_asc_I4.c \
	./AUTOGEN/GENFILES/bin_search_I4.c ${LINKFLAGS}

*/
#ifdef IPP
#include "ipps.h"
#include "ippcore.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "auxil.h"
#include "bin_search_I4.h"
#include "qsort_asc_I4.h"


#ifndef IPP
#define ippGetCpuClocks get_time_usec
#endif

int
main()
{
  int status = 0;
  int mid;
  mid = 418001; // INPUT 
  mid = 457794; // INPUT 

  char *TM_mid_file = "/mnt/u001/Q/PETER/_tempf_oSjB7I";
  char *TM_TC_lb_file = "/mnt/u001/Q/PETER/_tempf_vvdPAN";
  char *TM_TC_ub_file = "/mnt/u001/Q/PETER/_tempf_yI7PqO";

  char *TC_to_file  = "/mnt/u001/Q/PETER/_tempf_8SFp9f";
  char *TC_to_idx_file  = "/mnt/u001/Q/PETER/_tempf_wsysGM";

  char *TM_mid_X = NULL;    size_t TM_mid_nX = 0; 
  char *TM_TC_lb_X = NULL;  size_t TM_TC_lb_nX = 0; 
  char *TM_TC_ub_X = NULL;  size_t TM_TC_ub_nX = 0; 

  char *TC_to_X = NULL;     size_t TC_to_nX = 0; 
  char *TC_to_idx_X = NULL; size_t TC_to_idx_nX = 0; 

  int *midI4 = NULL, *toI4 = NULL, *to_idxI4 = NULL;
  long long *TC_lbI4 = NULL, *TC_ubI4 = NULL;
  long long mid_idx;
  int n_deg_1 = 0;
  int deg1[65536]; // 65536 is max number of connections
  long long num1[65536]; // 65536 is max number of connections
  long long lb1[65536]; // 65536 is max number of connections
  long long ub1[65536]; // 65536 is max number of connections
  int conn2[1048576]; // max number of unsorted 2nd degree
  int dedupe_conn2[1048576]; // max number of de-duped 2nd degree

  status = rs_mmap(TM_mid_file, &TM_mid_X, &TM_mid_nX, 0); cBYE(status);
  midI4 = (int *)TM_mid_X;

  status = rs_mmap(TM_TC_lb_file, &TM_TC_lb_X, &TM_TC_lb_nX, 0); cBYE(status);
  TC_lbI4 = (long long *)TM_TC_lb_X;

  status = rs_mmap(TM_TC_ub_file, &TM_TC_ub_X, &TM_TC_ub_nX, 0); cBYE(status);
  TC_ubI4 = (long long *)TM_TC_ub_X;

  status = rs_mmap(TC_to_file,  &TC_to_X, &TC_to_nX, 0); cBYE(status);
  toI4  = (int *)TC_to_X;

  status = rs_mmap(TC_to_idx_file,  &TC_to_idx_X, &TC_to_idx_nX, 0); cBYE(status);
  to_idxI4  = (int *)TC_to_idx_X;

  long long nM = TM_mid_nX / sizeof(int);
  long long nC = TC_to_nX / sizeof(int);

  status = bin_search_I4(midI4, nM, mid, &mid_idx, "");
  if ( mid_idx < 0 ) { go_BYE(-1); }


  long long lb0 = TC_lbI4[mid_idx];
  long long ub0 = TC_ubI4[mid_idx];
  if ( ub0 <= lb0 ) { go_BYE(-1); }
  n_deg_1 = ub0 - lb0;
  fprintf(stderr, "Number of 1 degree connections = %d \n", n_deg_1);
  unsigned long long t0 = ippGetCpuClocks();
#ifdef IPP
  ippsCopy_32s(toI4+lb0, deg1, n_deg_1);
#else
  memcpy(deg1, toI4+lb0, n_deg_1*sizeof(int));
#endif
  unsigned long long t1 = ippGetCpuClocks();

  int cnt = 0;
  for ( int i = 0; i < n_deg_1; i++ ) {
    /* THIS IS CORRECT BUT SLOWER
    int l_mid = deg1[i];
    status = bin_search_I4(midI4, nM, l_mid, &mid_idx, "");
    if ( mid_idx < 0 ) { go_BYE(-1); }
    */
    mid_idx = to_idxI4[i];
    lb1[i] = TC_lbI4[mid_idx];
    ub1[i] = TC_ubI4[mid_idx];
    cnt += ub1[i] - lb1[i];
  }
  unsigned long long t2 = ippGetCpuClocks();
  fprintf(stderr, "Need to process %d people \n", cnt);

  int j = 0;
  // Could be Cilk-ified
  for ( int i = 0; i < n_deg_1; i++ ) {
    long long lb = lb1[i];
    long long ub = ub1[i];
    memcpy(conn2+j, toI4+lb, (ub-lb)*sizeof(int));
    j += (ub-lb);
  }
  if ( j != cnt ) { go_BYE(-1); }
  unsigned long long t3 = ippGetCpuClocks();

  /*
  int num_blocks = 64;
  int block_size = cnt / num_blocks;
  cilkfor ( int i = 0; i < num_blocks; i++ ) { 
    int lb = i * block_size;
    int ub = lb + block_size;
    if ( i == (num_blocks-1) ) { ub = cnt; }
    qsort_asc_I4 (conn2+lb, (ub-lb), sizeof(int), NULL); 
  }
  */
    
#ifdef IPP
  ippsSortAscend_32s_I(conn2, cnt);
#else
  qsort_asc_I4 (conn2, cnt, sizeof(int), NULL); 
#endif
  unsigned long long t4 = ippGetCpuClocks();
#define SIMPLE
#ifdef SIMPLE
  int curr_mid; 

  curr_mid = dedupe_conn2[0] = conn2[0];
  j = 1;
  for ( int i = 0; i < cnt; i++ ) { 
    if ( conn2[i] == curr_mid ) {
      curr_mid = conn2[i];
      dedupe_conn2[j++] = curr_mid;
    }
  }
#endif
  unsigned long long t5 = ippGetCpuClocks();
  t5 -= t4;
  t4 -= t3;
  t3 -= t2;
  t2 -= t1;
  t1 -= t0;
  fprintf(stderr, "t1 = %llu \n", t1);
  fprintf(stderr, "t2 = %llu \n", t2);
  fprintf(stderr, "t3 = %llu \n", t3);
  fprintf(stderr, "t4 = %llu \n", t4);
  fprintf(stderr, "t5 = %llu \n", t5);
#ifdef SIMPLE
  fprintf(stderr, "De-duped second degree = %d people \n", j);
#else
  fprintf(stderr, "De-duped second degree = %d people \n", new - dedupe_conn2);
#endif

BYE:
  return(status);
}

