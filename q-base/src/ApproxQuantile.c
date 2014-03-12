#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
// Maximum possible string length of incoming data
// // Set it to 25 unless really big strings
#define LEN 25
#define MIN_SZ 1048576
#include <time.h>
#include "assert.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "assign_I4.h"
#include "qsort_asc_I4.h"

#undef IPP
#define OPT

#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#endif

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//----------------------------------------------
// START FUNC DECL
int
determine_b_k(
	      double err,
	      int num_quantiles,
	      long long siz,
	      int *ptr_b,
	      int *ptr_k
	      )
// STOP FUNC DECL
{
  int status= 0;
  int b, k;

  if ( num_quantiles >= siz ) { go_BYE(-1); }
  b = 2; 

  while ((b-2)*pow(2,b-2)+1 < (int)(err*siz)) {
    b++;
  }
  b--;
  k=((int)(siz/pow(2,b-1)+1)/num_quantiles+1)*num_quantiles+1;

  if ( b <= 0 ) { go_BYE(-1); }
  if ( k <= 0 ) { go_BYE(-1); }
  *ptr_b = b;
  *ptr_k = k;

 BYE:
  return status ;
}
//----------------------------------------------

int flag=1;
int count_iter=0;

void printPoll(int location)
{
  if (location%1000000 == 0 && location>0)
    {
      printf(".");
      fflush(stdout);
    }
  if (location%10000000 == 0)
    {
      printf("%d", (int)(location/10000000));
      fflush(stdout);
    }
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

/* OLD 
   void New(int *value, int *buffer1, int *weight, int w1, int loc1, int k)
   {

   int ii;

   for (ii=0; ii<k; ii++)
   buffer1[ii]=value[ii];

   weight[loc1]=w1;

   }
*/

int New(
	int *src_buf, 
	int *dst_buf,
	int *weight,
	int initial_weight,
	int bufidx, 
	int bufsz
	)
{
  int status = 0;
  memcpy(dst_buf, src_buf, bufsz);
  weight[bufidx] = initial_weight ; // indicating buffer is now used 
 BYE:
  return status ;
}


int 
Collapse(
	 int *buffer1,  /* sorted ascending */
	 int *buffer2,  /* sorted ascending */
	 int *weight, 
	 int loc1, 
	 int loc2, 
	 int k
	 )
{
  int status = 0;
  int* tempBuffer = NULL;
  int* tempWeight = NULL;

  int outWeight = weight[loc1]+weight[loc2];
  int offSet = (outWeight+2*flag)/2;
  flag=1-flag;

  tempBuffer = (int *)malloc(2*k*sizeof(int));
  tempWeight = (int *)malloc(2*k*sizeof(int));

  int ii=0, jj=0, kk=0;

  while(1)
    {
      if (ii<k && jj<k)
	{
	  if (buffer1[ii] <= buffer2[jj])
	    {
	      tempBuffer[kk]=buffer1[ii];
	      tempWeight[kk]=weight[loc1];
	      ii++;
	      kk++;
	    }
	  else
	    {
	      tempBuffer[kk]=buffer2[jj];
	      tempWeight[kk]=weight[loc2];
	      jj++;
	      kk++;
	    }
	}

      if (ii<k && jj==k)
	{
	  tempBuffer[kk]=buffer1[ii];
	  tempWeight[kk]=weight[loc1];
	  ii++;
	  kk++;
	}

      if (ii==k && jj<k)
	{
	  tempBuffer[kk]=buffer2[jj];
	  tempWeight[kk]=weight[loc2];
	  jj++;
	  kk++;
	}

      if (ii==k && jj==k)
	break;

    }

  int currentIndex = 0;
  ii=0;
  jj=0;
  
  while (ii<2*k)
    {
      currentIndex+=tempWeight[ii];

      if (currentIndex >= offSet+jj*outWeight)
	{
	  buffer1[jj]=tempBuffer[ii];
	  jj++;
	}

      ii++;
    }
  weight[loc1]=outWeight;
  weight[loc2]=0;
  

 BYE:
  free_if_non_null(tempBuffer);
  free_if_non_null(tempWeight);
  return status ;
}


// START FUNC DECL
int 
approx_quantile(
		int *x, /* input data */
		long long siz, /* number of elements */
		int num_quantiles, /* number of quantiles */
		double err, /* acceptable error */
		int *y /* output */
		)
// STOP FUNC DECL
{
  int status = 0;
  int b, k; /* There are b buffers with k elements each */
  int *valueBuffer = NULL;
  int **buffer = NULL;
  int *weight = NULL;

  if ( siz < MIN_SZ ) { go_BYE(-1); }
#ifdef IPP
  if ( siz >= INT_MAX ) { go_BYE(-1); }
#endif
  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( y == NULL ) { go_BYE(-1); }
  if ( ( err <= 0 )  || ( err >= 1 ) ) { go_BYE(-1); }
  if ( siz < 2 ) { go_BYE(-1); }
  if ( ( num_quantiles < 1 ) || ( num_quantiles >= siz ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = determine_b_k(err, num_quantiles, siz, &b, &k); cBYE(status);
  
  int count = 1;
  int no_of_empty_buffers = b;

  // START: Allocate buffers
  valueBuffer = malloc(k * sizeof(int));
  return_if_malloc_failed(valueBuffer);
  buffer      = malloc(b * sizeof(int *));
  return_if_malloc_failed(buffer);
  weight      = malloc(b * sizeof(int));
  return_if_malloc_failed(weight);

  for ( int ii=0; ii<b; ii++) {
    buffer[ii] = malloc(k*sizeof(int));
    return_if_malloc_failed(buffer[ii]);
    // Initialize to 0 
#ifdef OPT
#ifdef IPP
    ippsZero_32s((int *)buffer[ii], k);
#else
    assign_const_I4(buffer[ii], k, 0); 
#endif
#else
    for (int jj=0; jj<k; jj++) { buffer[ii][jj] = 0; }
#endif
    weight[ii] = 0;
  }
  // STOP: Allocate buffers

  // Sort entire array in chunks 
  int num_chunks = siz / k;

  cilkfor ( int ii = 0; ii < num_chunks; ii++ ) { 
    int *lX = x + (ii*k); 
#ifdef OPT
#ifdef IPP
    ippsSortAscend_32s_I(lX, k);
#else
    qsort_asc_I4(lX, k, sizeof(int), NULL);
#endif
#else
    qsort(lX, k, sizeof(int),compare);
#endif
  }

  int bufsz = k * sizeof(int); 

  // Note that since siz is not a multiple of k, we ignore last few
  count = 1;
  while ( count*k < siz ) {
    /* Copy the "count"^{th} block from x into  valueBuffer */
#ifdef OPT
#ifdef IPP
    ippsCopy_32s(x + (k * (count-1)), valueBuffer, k);
#else
    memcpy(valueBuffer, x + (k * (count-1)), bufsz);
#endif
#else
    for ( int ii=0; ii<k; ii++) { 
      valueBuffer[ii]=x[(count-1)*k+ii]; 
    }
#endif

    // if no buffer available, merge 2 buffers into 1 and free up other
    if ( no_of_empty_buffers == 0 ) {
      // find 2 buffers with same weight
      bool found = false;
      int buf1 = -1, buf2 = -1; // two buffers to merge
      for ( int ii = 0; ii < b-1; ii++ ) { 
	for ( int jj = ii+1; jj < b; jj++ ) { 
	  if ( weight[ii] == weight[jj] ) { 
	    buf1 = ii; buf2 = jj;
	    found = true;
	    break;
	  }
	}
      }
      if ( found == false ) { go_BYE(-1); }
      // collapse buffer buf1 and buffer buf2
      status = Collapse(buffer[buf1],buffer[buf2], weight, buf1, buf2, k);
      cBYE(status);
      no_of_empty_buffers++;
    }
    //----------------------------------------------
    /* Copty the valueBuffer into the first free buffer, free being
     * determined by weight = 0 */
    bool found = false;
    for ( int ii = 0; ii < b; ii++) {
      if ( weight[ii] == 0 ) {
	// OLD New(valueBuffer, buffer[ii], weight, 1, ii, bufsz);
	New(valueBuffer, buffer[ii], weight, 1, ii, bufsz);
        no_of_empty_buffers--;
	found = true;
	break;
      }
    }
    if ( found == false ) { go_BYE(-1); }
    count++;
  }
  //------------------------------------------------------
  for ( int ii = b-1; ii>0; ii--) {
    status = Collapse(buffer[ii-1],buffer[ii],weight, ii-1, ii,k);
    cBYE(status);
  }

  for ( int ii=0; ii<=num_quantiles; ii++) {
    y[ii]=buffer[0][ii*(k-1)/num_quantiles];
  }
 BYE:
  free_if_non_null(valueBuffer);
  if ( buffer != NULL ) { 
    for ( int ii=0; ii<b; ii++) {
      free_if_non_null(buffer[ii]);
    }
  }
  free_if_non_null(buffer);
  free_if_non_null(weight);
  return status ;
}
