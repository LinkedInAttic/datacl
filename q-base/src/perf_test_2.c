// compile as 
// 
/* 
   gcc perf_test_2.c -O4 -std=gnu99 mmap.c \
   -I./AUTOGEN/GENFILES/ auxil.c ./AUTOGEN/GENFILES/assign_I1.c \
   ./AUTOGEN/GENFILES/qsort_asc_I4.c ./AUTOGEN/GENFILES/bin_search_I4.c -I. -lm

   icc perf_test_2.c -O3 -std=c99 mmap.c \
   -I./AUTOGEN/GENFILES/ auxil.c ./AUTOGEN/GENFILES/assign_I1.c \
   ./AUTOGEN/GENFILES/qsort_asc_I4.c ./AUTOGEN/GENFILES/bin_search_I4.c -I. -DIPP -ipp
*/
// Aim is to test f1s1opf2 in different ways

#include "qtypes.h"
#include <omp.h>
// #include "ipps.h"
#include "auxil.h"
#include "mmap.h"
#include "qsort_asc_I4.h"
#include "bin_search_I4.h"

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
  int num_scalars = -1;
  int *scalars = NULL;
  int *hash_table = NULL;
  int *temp_vec = NULL;
  int vec_length = 8192; int nB;

  fprintf(stderr, "=========================================\n");
  int mode = -1;
  if ( argc != 3 ) { go_BYE(-1); }
  status = stoI4(argv[1], &mode); cBYE(status);
  status = stoI4(argv[2], &num_scalars); cBYE(status);
  scalars = malloc(num_scalars * sizeof(int));
  unsigned int hash_table_size = prime_geq(num_scalars * num_scalars);
  hash_table_size = 4096; // assumes num scalars <= 64 
  hash_table = malloc(hash_table_size * sizeof(int));
  unsigned int a = prime_geq(rand() % 1048576);
  unsigned int b = prime_geq(rand() % 1048576);
  temp_vec = malloc(2*vec_length * sizeof(int)); 
  int *XI4 = NULL;

  status = rs_mmap(xfile, &X, &nX, 0); cBYE(status);
  status = rs_mmap(yfile, &Y, &nY, 0); cBYE(status);
  status = rs_mmap(zfile, &Z, &nZ, 1); cBYE(status);

  long long nR = nX / sizeof(int);
  int nT = 64;
  int block_size = nR / nT;
  // omp_set_num_threads(8);
  int num_compares = 0;
  int num_equals   = 0;

  for ( int i = 0; i < num_scalars; i++ ) { 
    scalars[i] = ((int *)X)[prime_geq(rand() % nR)];
    scalars[i] = i+1; 
  }
  qsort_asc_I4(scalars, num_scalars, sizeof(int), NULL);
  /*
    for ( int i = 0; i < num_scalars; i++ ) { 
    fprintf(stderr, "%d => %d \n", i, scalars[i] );
    }
  */

  switch ( mode ) { 
  case 4 : 
    for ( int tid = 0; tid < nT; tid++ ) {
      int lb = tid * block_size;
      int ub = lb  + block_size;
      if ( tid == (nT-1) ) { ub = nR; }
      long long nX = ub - lb;
      int  *XI4 = (int  *)X;  XI4 += lb;  
      int  *YI4 = (int  *)Y;  YI4 += lb;
      char *ZI1 = (char *)Z;  ZI1 += lb;
      // START: Here is the real code 
      for ( long long i = 0; i < nX; i++ ) { 
	long long pos;
        status = bin_search_I4(scalars, num_scalars, *XI4, &pos, "");
	if ( pos >= 0 ) { 
	  *ZI1 = 1;
	  num_equals++; 
	}
	else {
	  *ZI1 = 0;
	}
	ZI1++;
	XI4++;
      }
    }
    break;
  case 1 : 
    XI4 = (int *)X;
    for ( long long i = 0; i < nR; i++ ) { 
      *Z = 0; // default initialization
	int val = *XI4;
      for ( int j = 0; j < num_scalars; j++ ) { 
	num_compares++;
	if ( val == scalars[j] ) { 
	  *Z = 1;
	  num_equals++;
	  break;
	}
      }
      Z++;
      XI4++;
    }
    break;
  case 2 : 
    nB = nR / vec_length;
    for ( int b = 0; b < nB; b++ ) { 
      int blb = b * vec_length;
      int bub = blb  + vec_length;
      if ( b == (nB-1) ) { bub = nR; }
      int bnX = bub - blb;
      char *bZI1 = (char *)Z;   bZI1 += blb;
      // Initialize to 0 
#pragma simd
      for (  int k = 0; k < bnX; k++ ) { 
	*bZI1 = 0;
	bZI1++;
      }
      //-------------------------------------------
      for ( int j = 0; j < num_scalars; j++ ) {
        int  *bXI4 = (int  *)X; bXI4 += blb;
              bZI1 = (char *)Z; bZI1 += blb;
	int scalar = scalars[j];
#pragma simd
	for (  int k = 0; k < bnX; k++ ) { 
	  int valI4 = *bXI4;
	  if ( valI4 == scalar ) { 
	    *bZI1 = 1;
	  }
	  bXI4++;
	  bZI1++;
	}
      }
    }
    break;
  case 3 : 
#pragma simd
      for ( int i = 0; i < nX; i++ ) { 
	int  *XI4 = (int  *)X; 
        char *ZI1 = (char *)Z; 
	int loc, valI4, k;
	valI4 = k = *XI4;
	/*
        k *= 357913941;
        k ^= k << 24;
        k += ~357913941;
        k ^= k >> 31;
        k ^= k << 31;
	loc = k & 0x00000FFF;
	*/
	// int loc = ( valI4 * a + b ) % hash_table_size;
	loc = ( (valI4 * a) + b ) & 0x00000FFF;
	if ( hash_table[loc] == valI4 ) {
	  *ZI1 = 1;
	}
	else {
	  *ZI1 = 0;
	}
	XI4++;
	ZI1++;
      }
    break;
  default : 
    go_BYE(-1);
    break;

  }
  fprintf(stderr, "nR           = %lld \n", nR); 
  fprintf(stderr, "Mode         = %d \n", mode);
  fprintf(stderr, "num_scalars  = %d \n", num_scalars);
  fprintf(stderr, "num_compares = %d \n", num_compares);
  fprintf(stderr, "num_equals   = %d \n", num_equals  );
  
 BYE:
  free_if_non_null(scalars);
  free_if_non_null(hash_table);
  free_if_non_null(temp_vec);
  rs_munmap(X, nX);
  rs_munmap(Y, nY);
  rs_munmap(Z, nZ);
  return(status);
}

/*
  int valI4 = XI4[i];
  int loc = ( valI4 * a + b ) % hash_table_size;
  if ( hash_table[loc] == valI4 ) { 
  t_num_equals++;
  }
*/
