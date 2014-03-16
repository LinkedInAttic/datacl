#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda_runtime.h>
#include <helper_cuda.h>
#include <helper_functions.h> // helper functions for SDK examples

#include <string>

//#include <bitset>
#include "filter.h"
using namespace std;

#define N 1000000

typedef long long myType;

/*
template <class T>
struct opFunctor{
        private:
                int op;
        public:
		opFunctor() { op = "eq"; }
		opFunctor(string opIn) { op = opIn; }
		__device__ bool operator() (const T &a, const T &b)
		{
			if( op == "lt" )
				return (a < b);
			else if( op == "le" )
				return (a <= b);
			else if( op == "gt")
				return (a > b);
			else if( op == "ge")
				return (a >= b);
			else if( op == "ne")
				return (a != b);
			else
				//by default, check for equal
				return (a == b);
		}
};
*/

template <class T>
struct opGreater{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a > b); }
};

template <class T, class Compare> 
__global__ void filterKernel(T *data, size_t dataSize, const T val, char *bitvec, Compare comp) 
{ 
 
        long long i = blockDim.x * blockIdx.x + threadIdx.x; 
 
        if(i < dataSize) 
        { 
                if( comp(data[i], val) ) 
                        bitvec[i] = '1'; 
                else 
                        bitvec[i] = '0'; 
        } 
} 


int main(int argc, char **argv)
{
	srand(time(NULL));
	struct timeval start, end;


//	myType data[N];
//	bitset<N> bitvec;
//	char bitvec[N];

//	for(long long i=0; i<N; i++)
//		data[i] = rand() % N;

//	myType val = rand() % N;


//    cudaError_t error;
    cudaDeviceProp deviceProp;

    // This will pick the best possible CUDA capable device
    int devID = findCudaDevice(argc, (const char **)argv);
    
    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));
    

    myType *h_data = (myType *) malloc(N * sizeof(myType));

    // initalize the memory
    for (long long i = 0; i < N; ++i)
    {
	    h_data[i] = i;
    }

    myType val = 1000;

    gettimeofday(&start, NULL);
    // allocate device memory
    myType *d_data;
    checkCudaErrors(cudaMalloc((void **) &d_data, N * sizeof(myType) ));
    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_data, h_data, N * sizeof(myType), cudaMemcpyHostToDevice));



    // allocate device memory for result
    char *d_bitvec;
    checkCudaErrors(cudaMalloc((void **) &d_bitvec, N * sizeof(char)));
    // allocate mem for the result on host side
    char *h_bitvec = (char *) malloc(N * sizeof(char));



    // Kernel configuration, where a one-dimensional
    // grid and one-dimensional blocks are configured.
    long long Nthreads = 1024;
    long long Nblocks = (N + Nthreads - 1 ) / Nthreads;
    dim3 dimGrid(Nblocks);
    dim3 dimBlock(Nthreads);

    //opFunctor<myType> opNew;

    // execute the kernel
    filterKernel<<< dimGrid, dimBlock >>>(d_data, N, val, d_bitvec, opGreater<myType>());

    cudaThreadSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");

    // copy result from device to host
    checkCudaErrors(cudaMemcpy(h_bitvec, d_bitvec, N * sizeof(char), cudaMemcpyDeviceToHost));

    cudaFree(d_data);
    cudaFree(d_bitvec);
    gettimeofday(&end, NULL);
    double timeDiff_us = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    cout << "Time Taken by GPU: " << timeDiff_us / 1000 << "ms" << endl;

    cudaDeviceReset();






/*
//Available comparator object functions (functors)
equal_to
not_equal_to
greater
greater_equal
less
less_equal
*/




    char *bitvec = (char *) malloc(N * sizeof(char));

    gettimeofday(&start, NULL);
    filter<myType> (h_data, (size_t)N, val, bitvec, greater<myType> ());
    gettimeofday(&end, NULL);

    for(long long i=0; i < N; i++)
    {
	    if(bitvec[i] != h_bitvec[i])
	    {
		    cout << "Results dont match!!! at" << i << endl;
		    break;
	    }
    }

    timeDiff_us = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    cout << "Time Taken by CPU: " << timeDiff_us / 1000 << "ms" << endl;

//Print results to check if they are correct.
//	for(long long i=0; i<N; i++)
//		cout << "(" << data[i] << " " << bitvec[i] << ")" << endl;
//	cout << endl << "Value = " << val << endl;

return 0;

}
