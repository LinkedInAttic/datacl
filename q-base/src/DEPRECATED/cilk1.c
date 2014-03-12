#include <stdio.h>
#include <math.h>
#include <stdlib.h>
extern long int random(void);

int
main(
    )
{
  int *X = NULL;
  int N = 1024;
  int M = 1048576;
  X = malloc(M * N * sizeof(int));
#ifdef CILK
#define __FOR__ _Cilk_for
#else
#define __FOR__ for
#endif
  __FOR__ ( int i = 0; i < M*N; i++ ) { 
    double k = i * N + sqrt((double)i);
    X[i] = floor(asin(sin((( (int)(k * k) % 31 ) / 32.0))));
  }
}
