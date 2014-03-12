// compile as 
// gcc perf_test_3.c -O4 -std=gnu99 mmap.c 
// icc perf_test_1.c -O3 -std=c99   mmap.c 
#include "qtypes.h"
#include <omp.h>
// #include "ipps.h"
#include "auxil.h"
#include "mmap.h"

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
  int  *XI4 = (int  *)X;
  int  *ZI4 = (int  *)Z;
  char *ZI1 = (char *)Z;
  long long nR = nX / sizeof(int);
  long long sum = 0;


  /* FOLLOWING CODE SELECTS EVERY ALTERNATE ROW 
  for ( int i = 0; i < nR; i++ ) { 
    if ( ( i % 2 ) == 0 ) {
      ZI1[i] = -1;
      sum++;
    }
    else {
      ZI1[i] = 0;
    }
  }
  fprintf(stdout, "sum = %lld \n", sum);
  go_BYE(-1);
  */

  int block_size = 65536;
  long long nB = nR / block_size;


  for ( long long b = 0; b < nB; b++ ) { 
    long long lb = b  * block_size;
    long long ub = lb + block_size;
    if ( b == (nB-1) ) { ub = nR; }
#pragma simd reduction(+:sum)
    for ( long long j = lb; j < ub; j++ ) { 
      int xvalI4 = XI4[j];
      char zvalI1 = ZI1[j];
      int zvalI4 = zvalI1;
      // sum += xvalI4; // OPTION 1 
      // sum += ( xvalI4 & zvalI4 ); // OPTION 3
      if ( zvalI1 == 1 ) { sum += xvalI4; } // OPTION 2 
    }
  }
  fprintf(stdout, "sum = %lld \n", sum);
  
BYE:
  return(status);
}

