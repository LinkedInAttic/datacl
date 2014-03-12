#include <stdio.h>
#include <stdlib.h>
#include "ipp.h"
#include "ipps.h"
#include "ippi.h"
/* DO NOT AUTO GENERATE HEADER FILE  */

main()
{
#define N 8
  char *X;
  X = malloc(N * sizeof(char));
  float *fY; int *iY;
  fY = malloc(N * sizeof(float));
  iY = malloc(N * sizeof(int));
  float *fZ; int *iZ;
  fZ = malloc(N * sizeof(float));
  iZ = malloc(N * sizeof(int));
  int ival_I4; float fval_F4;

  ippsSet_8u('X', X, N);
  ival_I4 = 10;
  ippsSet_32s(ival_I4, iY, N);
  ippsSet_32f(1.0, fY, N);

  for ( int i = 0; i < N; i++ ) { 
    fprintf(stderr, "%d: %c \n", i, X[i]);
  }
  // ippmAdd_vc_32f(Y, 4, 10.0, Z, 4, N);
  ippsAddC_32f(fY, (float)10.0, fZ, N);
  for ( int i = 0; i < N; i++ ) { 
    fprintf(stderr, "%d: %f --> %f \n", i, fY[i], fZ[i]);
  }

  ippsAddC_32s_Sfs(iY, ival_I4, iZ, (int)N, 0);
  ippsSubC_32s_Sfs(iY, ival_I4, iZ, (int)N, 0);
  ippsMulC_32s_Sfs(iY, ival_I4, iZ, (int)N, 0);

  ival_I4 = 257;
  ippsSet_32s(ival_I4, iY, N);
  ival_I4 = 16;
  for ( int i = 0; i < N; i++ ) { 
    fprintf(stderr, "%d: %d --> %d \n", i, iY[i], iZ[i]);
  }
}
