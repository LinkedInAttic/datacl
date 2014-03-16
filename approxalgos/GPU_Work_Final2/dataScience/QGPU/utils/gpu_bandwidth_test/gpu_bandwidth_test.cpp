#include<sys/time.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <helper_functions.h> // helper functions for SDK examples

using namespace std;

#define tran_GPU2CPU 1
//#define cudaTIMER
#define NUMITER 1

#define TIMETHIS(command, startTimer, endTimer) \
gettimeofday(startTimer, NULL);                 \
command;                                        \
gettimeofday(endTimer, NULL);                   \


#define CU_TIMETHIS(command, startTimer, stopTimer, elapsedTime) \
checkCudaErrors(cudaEventRecord(startTimer, 0));								\
command;																												\
checkCudaErrors(cudaEventRecord(stopTimer, 0));									\
checkCudaErrors(cudaDeviceSynchronize());												\
checkCudaErrors(cudaEventElapsedTime(&elapsedTimeInMs, startTimer, stopTimer));		\

typedef long long myType;

/**
* @brief get time difference between timers star and end
*
* @param start start timer - has to be struct timeval pointer
* @param end end timer - has to be struct timeval pointer
*
* @return time difference in millisecond
*/
double getTimeDiff( struct timeval& start, struct timeval& end )
{
	return ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000 ;
}

int main()
{
	int min2Pow = 20; // starting with 1MB
	int max2Pow = 32; // ending at 4GB
	int numSizes = max2Pow - min2Pow + 1;

	float elapsedTimeInMs = 0.0f;


	cudaEvent_t cu_start, cu_stop;
	checkCudaErrors(cudaEventCreate(&cu_start));
	checkCudaErrors(cudaEventCreate(&cu_stop));


	long long *dataSizes = (long long *) malloc( numSizes * sizeof(long long) );

	for(int i=0; i<numSizes; dataSizes[i]=pow(2, min2Pow + i), i++);

	char * d_data;
	for(int i=0; i<numSizes; i++)
	{
		struct timeval start, end;

		cout << "-----------------------------------------------------" << endl;

		cout << "Data Size = " << dataSizes[i] << " bytes" << endl;
		long long dataSize = dataSizes[i];

		char *h_data = (char *) malloc( dataSize * sizeof(char) );
		TIMETHIS ( checkCudaErrors(cudaMalloc( (void **) &d_data, dataSize * sizeof(char) )), &start, &end );

		cout << "Time Taken for alloc: " << getTimeDiff(start, end) << "ms" << endl;

#ifdef cudaTIMER

		CU_TIMETHIS( for(int i=0; i<NUMITER; i++, checkCudaErrors(cudaMemcpy(d_data, h_data, dataSize * sizeof(char), cudaMemcpyHostToDevice))), cu_start, cu_stop, elapsedTime );

		cout << "Time for " << NUMITER << " CPU2GPU transfers: " << elapsedTimeInMs << "ms, ";
		cout << "Bandwidth: " << dataSize / (elapsedTimeInMs/NUMITER) * 1000 << " bytes/s" << endl;

#else

		TIMETHIS( for(int i=0; i<NUMITER; i++, checkCudaErrors(cudaMemcpy(d_data, h_data, dataSize * sizeof(char), cudaMemcpyHostToDevice))), &start, &end );

		cout << "Time for " << NUMITER << " CPU2GPU transfers: " << getTimeDiff(start, end) << "ms, ";
		cout << "Bandwidth: " << dataSize / (getTimeDiff(start, end) / 1000 / NUMITER) << " bytes/s" << endl;
#endif


		if(tran_GPU2CPU)
		{
#ifdef cudaTIMER

			CU_TIMETHIS( for(int i=0; i<NUMITER; i++, checkCudaErrors(cudaMemcpy(d_data, h_data, dataSize * sizeof(char), cudaMemcpyHostToDevice))), cu_start, cu_stop, elapsedTime );

		cout << "Time for " << NUMITER << " CPU2GPU transfers: " << elapsedTimeInMs << "ms, ";
		cout << "Bandwidth: " << dataSize / (elapsedTimeInMs/NUMITER) * 1000 << " bytes/s" << endl;

#else

			TIMETHIS( for(int i=0; i<NUMITER; i++, checkCudaErrors(cudaMemcpy(h_data, d_data, dataSize * sizeof(char), cudaMemcpyDeviceToHost))), &start, &end );
		cout << "Time for " << NUMITER << " GPU2CPU transfers: " << getTimeDiff(start, end) << "ms, ";
		cout << "Bandwidth: " << dataSize / (getTimeDiff(start, end) / 1000 / NUMITER) << " bytes/s" << endl;
#endif

		}


		cout << "-----------------------------------------------------" << endl << endl;


		cudaFree( d_data );
		free( h_data );

	}
}
