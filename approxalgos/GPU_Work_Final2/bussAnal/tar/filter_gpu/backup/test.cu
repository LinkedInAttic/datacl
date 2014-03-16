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

#define N 100000000
#define NCHAR 20
#define NTHREADS 512
#define NBLOCKS 512

enum op_t {EQ, LT, LE, GT, GE, NE};

typedef long long myType;

template <typename T>
struct opFunctor{
        private:
                int op;
        public:
		opFunctor() { op = EQ; }
		opFunctor(int opIn) { op = opIn; }
		__host__ __device__ void setOp (int opIn) { op = opIn; }
		__host__ __device__ bool operator() (const T &a, const T &b)
		{
			if( op == EQ ) //eq
				return (a == b);
			else if( op == LT ) //lt
				return (a < b);
			else if( op == LE ) //"le") )
				return (a <= b);
			else if( op == GT ) //"gt") )
				return (a > b);
			else if( op == GE ) //"ge") )
				return (a >= b);
			else if( op == NE ) //"ne") )
				return (a != b);
			else
				//by default, check for equal
				return (a == b);
		}
};

//Functors for 2-obj comparison
template <typename T>
struct opGreater{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a > b); }
};

template <typename T>
struct opGreaterEqual{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a >= b); }
};

template <typename T>
struct opLesser{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a < b); }
};

template <typename T>
struct opLesserEqual{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a <= b); }
};

template <typename T>
struct opNotEqual{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a != b); }
};

template <typename T>
struct opEqual{
        public:
		__host__ __device__ bool operator() (const T &a, const T &b)
		{ return (a == b); }
};


//Filter GPU Kernel
template <typename T, class Compare> 
__global__ void filterGPU(T *data, size_t dataSize, const T val, char *bitvec, Compare comp) 
{
	//1-D thread index
        long long i = blockDim.x * blockIdx.x + threadIdx.x; 
 
	while(i < dataSize)
	{
		if( comp(data[i], val) ) 
			bitvec[i] = '1'; 
		else 
			bitvec[i] = '0'; 
		i += blockDim.x * gridDim.x;
	}
} 


int main(int argc, char **argv)
{
    struct timeval start, end;
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

    // start timer
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
    dim3 dimGrid(NBLOCKS);
    dim3 dimBlock(NTHREADS);

    // declare functor for comparator callback function
    opFunctor<myType> opFuncNew;
    op_t opNew = GT;
    opFuncNew.setOp(opNew);

    // execute the kernel
    filterGPU<<< dimGrid, dimBlock >>>(d_data, N, val, d_bitvec, opFuncNew);

    cudaThreadSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");

    // copy result from device to host
    checkCudaErrors(cudaMemcpy(h_bitvec, d_bitvec, N * sizeof(char), cudaMemcpyDeviceToHost));

    gettimeofday(&end, NULL);
    double timeDiff_us = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    cout << "Time Taken by GPU: " << timeDiff_us / 1000 << "ms" << endl;

    // free device memory
    cudaFree(d_data);
    cudaFree(d_bitvec);

    cudaDeviceReset();


    // result array for verification
    char *bitvec = (char *) malloc(N * sizeof(char));

    gettimeofday(&start, NULL);
    // run host code for comparison
    filter<myType> (h_data, (size_t)N, val, bitvec, opFuncNew);
    gettimeofday(&end, NULL);

    for(long long i=0; i < N; i++)
    {
	    if(bitvec[i] != h_bitvec[i])
	    {
		    cout << "Results dont match!!! at " << i << " gpu: " << bitvec[i] << " cpu: " << h_bitvec[i] << endl;
		    break;
	    }
    }

    timeDiff_us = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    cout << "Time Taken by CPU: " << timeDiff_us / 1000 << "ms" << endl;

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);

    return 0;

}
