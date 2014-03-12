// compile as 
// gcc perf_test_1.c -O4 -std=gnu99 -fopenmp -lgomp mmap.c ./AUTOGEN/GENFILES/add2_I4.c -I./AUTOGEN/GENFILES/ ./AUTOGEN/GENFILES/add2_F4.c 
// icc perf_test_1.c -O3 -std=c99 -fopenmp -lgomp ./AUTOGEN/GENFILES/add2_I4.c -I./AUTOGEN/GENFILES/ mmap.c  ./AUTOGEN/GENFILES/add2_F4.c 
#include "qtypes.h"
#include <omp.h>
// #include "ipps.h"
#include "auxil.h"
#include "mmap.h"
#include "add2_I4.h"
#include "add2_F4.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *X = NULL; size_t nX = 0; 
  char *xfile = "/dev/shm/DATA/randI4_1.bin";
  char *Y = NULL; size_t nY = 0;
  char *yfile = "/dev/shm/DATA/randI4_2.bin";
  char *Z = NULL; size_t nZ = 0;
  char *zfile = "/dev/shm/DATA/junkop.bin";

  status = rs_mmap(xfile, &X, &nX, 0); cBYE(status);
  status = rs_mmap(yfile, &Y, &nY, 0); cBYE(status);
  status = rs_mmap(zfile, &Z, &nZ, 1); cBYE(status);

#undef DATA_AS_INT

  long long nR = nX / sizeof(int);
  int nT = 64;
  int block_size = nR / nT;
  omp_set_num_threads(8);
#pragma omp parallel  for
//  _Cilk_for ( int tid = 0; tid < nT; tid++ ) { 
  for ( int tid = 0; tid < nT; tid++ ) { 
    int lb = tid * block_size;
    int ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
#ifdef DATA_AS_INT
    int *XI4 = (int *)X; XI4 += lb;
    int *YI4 = (int *)Y; YI4 += lb;
    int *ZI4 = (int *)Z; ZI4 += lb;
    add2_I4(XI4, YI4, block_size, ZI4);
#else
    float *XF4 = (float *)X; XF4 += lb;
    float *YF4 = (float *)Y; YF4 += lb;
    float *ZF4 = (float *)Z; ZF4 += lb;
    add2_F4(XF4, YF4, block_size, ZF4);
    // ippsAdd_32f(XF4, YF4, ZF4, (int)block_size);
#endif
  }
  
BYE:
  return(status);
}

