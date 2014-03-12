/* DO NOT AUTO GENERATE HEADER FILE  */
#include <time.h>
#include <stdbool.h>
#include "QLevelDB.h"
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"

// #define INPUT_FILE_NAME "testset.bin"
#define INPUT_FILE_NAME "conn.bin"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main()
{
  int status = 0;
  int *x = NULL;
  int *y = NULL;
  char *X = NULL; size_t nX = 0;

  int fd=open(INPUT_FILE_NAME,O_RDONLY);

  struct stat s;
  status = fstat(fd, &s);
  cBYE(status);
  long long siz=s.st_size/sizeof(int);	   
  clock_t begin, end;

  status = rs_mmap(INPUT_FILE_NAME, &X, &nX, 1); cBYE(status);
  x = (int *)X;
  if ( x == NULL ) { go_BYE(-1); }
  int num_quantile=100;
  y = (int *)malloc((num_quantile+1) * sizeof(int));
  double err=0.001;

  int ii;

  begin=clock();
  approx_quantile(x, siz, num_quantile, err, y);
  end=clock();

  for (ii=0; ii<=num_quantile; ii++)
    printf("\n%d:" ANSI_COLOR_GREEN"%d"ANSI_COLOR_RESET, ii,y[ii]);

  double time_spent=(double)(end-begin)/CLOCKS_PER_SEC;

  printf("\nTotal time taken:" ANSI_COLOR_RED "[%.3fs]\n" ANSI_COLOR_RESET, time_spent);
  
BYE:
  rs_munmap(X, nX);
  free_if_non_null(y);
  return(status);
}
