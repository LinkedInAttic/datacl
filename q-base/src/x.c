#include <stdio.h>
#include <stdlib.h>
#include "ipps.h"
#include "ippcore.h"


int
main()
{
  long long *X = NULL;
  FILE *fp = NULL;
  unsigned long long t1, t2;
  // fp = fopen("/dev/shm/_xxx", "wb");
  fp = fopen("/home/rsubramo/_xxx", "wb");
  X = malloc(1048576*sizeof(long long));
  for ( int i = 0; i < 1048576; i++ ) { 
    X[i] = i;
  }
  t1 = ippGetCpuClocks();
  for ( int i = 0; i < 32; i++ ) { 
    fwrite(X, 1048576, sizeof(long long), fp);
  }
  t2 =  ippGetCpuClocks();
  fclose(fp);
  fprintf(stderr, "time = %llu \n", t2 - t1);
}

