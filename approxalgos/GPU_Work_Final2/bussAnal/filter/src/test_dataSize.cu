/*

#include<sys/time.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <helper_functions.h> // helper functions for SDK examples


int main()
{
  int min2Pow = 20; // starting with 1MB
  int max2Pow = 32; // ending at 4GB
  int numSizes = max2Pow - min2Pow + 1;
	int ndict = 5000;

  long long *dataSizes = (long long *) malloc( numSizes * sizeof(long long) );

  for(int i=0; i<numSizes; dataSizes[i]=pow(2, min2Pow + i), i++);

  for(int i=0; i<numSizes; i++)
	{
		testfilter(dataSizes[i]);
	}
}

*/
