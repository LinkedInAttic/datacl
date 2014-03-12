#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include "constants.h"
#include "macros.h"
#include <fcntl.h>
#include <sys/mman.h>
#include "mmap.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define OUTPUT_FILE_NAME "../outdata.bin"
#define KEY_FILE_NAME_1  "../key1.bin"
#define KEY_FILE_NAME_2  "../key2.bin"
#define LEN_FILE_NAME    "../len.bin"
#define OFFSET_FILE_NAME "../offset.bin"

int main ()
{
  
  int status = 0;

  long long siz;

  int *x;
  char * X = NULL; size_t nX = 0; 

  status = rs_mmap(OUTPUT_FILE_NAME, &X, &nX, 0); cBYE(status);
  siz = nX / sizeof(int);
  x = (int *)X;

  printf("\nSize of OUTPUT_FILE: "ANSI_COLOR_GREEN"%lld \n"ANSI_COLOR_RESET,siz);
  for ( int ii = 0; ii < siz; ii++ ) {
    printf("%d: "ANSI_COLOR_GREEN "%d\n"ANSI_COLOR_RESET,ii, x[ii]);
  }
  
  //---------------------------------------------------------------------

  char *y;
  char * Y = NULL; size_t nY = 0;

  status = rs_mmap (KEY_FILE_NAME_1, &Y, &nY, 0); cBYE(status);
  siz = nY/ MAX_STR_LEN;
  
  y = (char *)Y;

  printf("\nSize of KEY_FILE_1: "ANSI_COLOR_GREEN"%lld \n"ANSI_COLOR_RESET,siz);

  for ( int ii = 0; ii < siz; ii++ ) { 

    char temp_str[MAX_STR_LEN];
    for ( int jj = 0; jj < MAX_STR_LEN; jj++ ) {
      temp_str[jj] = y[MAX_STR_LEN*ii+jj];
    }
    printf("%d: "ANSI_COLOR_GREEN "%s\n"ANSI_COLOR_RESET,ii, temp_str);

  }

  //----------------------------------------------------------------------

  char * z0;
  char * Z0 = NULL; size_t nZ0 = 0;

  int * z1;
  char * Z1 = NULL; size_t nZ1 = 0;

  int * z2;
  char * Z2 = NULL; size_t nZ2 = 0;

  status = rs_mmap (KEY_FILE_NAME_2, &Z0, &nZ0, 0); cBYE(status);
  status = rs_mmap (LEN_FILE_NAME, &Z1, &nZ1, 0); cBYE(status);
  status = rs_mmap (OFFSET_FILE_NAME, &Z2, &nZ2, 0); cBYE(status);
  
  siz = nZ1 / sizeof(int);
  
  z0 = (char *)Z0;
  z1 = (int *)Z1;
  z2 = (int *)Z2;

  printf("\nSize of KEY_FILE_2: "ANSI_COLOR_GREEN"%lld \n"ANSI_COLOR_RESET,siz);

  for ( int ii = 0; ii < siz; ii++ ) { 

    char* temp_str = (char *)malloc ( z1[ii]*sizeof(char) );
    printf(ANSI_COLOR_RED" (%d,%d) "ANSI_COLOR_RESET,z2[ii],z2[ii]+z1[ii]-1);
    for ( int jj = 0; jj < z1[ii]; jj++ ) {
      temp_str[jj] = z0[z2[ii]+jj];
    }
    printf("%d: "ANSI_COLOR_GREEN "%s\n"ANSI_COLOR_RESET,ii, temp_str);
    free(temp_str);

  }
  

 BYE:

  rs_munmap(X, nX);
  rs_munmap(Y, nY);

  return (status);

}
