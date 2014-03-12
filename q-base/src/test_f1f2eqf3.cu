#include <iostream>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <sys/time.h>
#include <helper_cuda.h>
#include <helper_functions.h> // helper functions for SDK examples
#include "/home/bpattabi/Work/bussAnal/filter/src/common.hpp"

#define TIMETHIS(command, startTimer, endTimer) \
  gettimeofday(startTimer, NULL);		\
  command;					\
  gettimeofday(endTimer, NULL);			\

#define NELEM 1e7
#define NTHREADS 512
#define NBLOCKS 512
#define CHUNK_SIZE 1e6


enum op_t {PLUS, MINUS};

typedef long long myType;

double getTimeDiff( 
		   struct timeval& start, 
		   struct timeval& end 
		    )
{
  return ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000 ;
}


// BP: Change input from integer to string
// BP: What happens if error in input? Handle this properly
// BP: Mus return status code of 0 if good; negative if bad 
template <typename T>
struct 
f1f2_functor
{
  char *op;
  f1f2_functor(char *in_op) : op(in_op) {}

  __host__ __device__
  T operator()(const T& x, const T& y) const
  {
    if ( strcmp(op, "+") == 0 ) { 
      return (x + y);
    }
    else if ( strcmp(op, "-") == 0 ) { 
      return (x - y);
    }
    else {
      // handle error case
    }
  }
};

template <typename T>
struct 
f1f2_functor
{
  int op;
  f1f2_functor(int in_op) : op(in_op) {}

  __host__ __device__
  T operator()(const T& x, const T& y) const
  {
    if(op == PLUS)
      return (x + y);
    else if(op == MINUS)
      return (x - y);
    else
      return (x + y);
  }
};

template <typename T, class BinaryFunction>
void 
transform_thrust(
		 T* f1, 
		 T* f2, 
		 T* f3, 
		 size_t nelem,
		 BinaryFunction bin_op
		 )
{
  //this device mem alloc takes finite time
  thrust::device_vector<T> d_f3 (nelem);

  thrust::device_vector<T> d_f1 (f1, f1 + nelem);// = h_f1;
  thrust::device_vector<T> d_f2 (f2, f2 + nelem);// = h_f2;

  thrust::transform(d_f1.begin(), d_f1.end(), d_f2.begin(), d_f3.begin(), bin_op);

  thrust::copy(d_f3.begin(), d_f3.end(), f3);
}


template <typename T, class BinaryFunction>
__global__ void 
gpu_add2_I4(
	    T* d_f1, 
	    T* d_f2, 
	    T* d_f3, 
	    size_t nelem
	    )
{
  //1-D thread index
  long long tid = blockDim.x * blockIdx.x + threadIdx.x;
  while(tid < nelem)
    {
      d_f3[tid] = d_f1[tid] + d_f2[tid];
      tid += blockDim.x * gridDim.x;
    }
}

template <typename T>
void 
transform_GPU_streams_wrap(
			   T* f1, 
			   T* f2, 
			   T* f3, 
			   size_t nelem, 
			   f1f2_functor<T> bin_op
			   )
{
  float elapsedTimeInMs = 0.0f;
  cudaEvent_t cu_start, cu_stop;
  checkCudaErrors(cudaEventCreate(&cu_start));
  checkCudaErrors(cudaEventCreate(&cu_stop));

  checkCudaErrors(cudaEventRecord(cu_start, 0));

  cudaDeviceProp deviceProp;
  // This will pick the best possible CUDA capable device
  int devID = findCudaDevice(0, NULL);
  checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));



  // Kernel configuration, where a one-dimensional
  // grid and one-dimensional blocks are configured.
  dim3 dimGrid(NBLOCKS);
  dim3 dimBlock(NTHREADS);

  T *h_f1 = f1;
  T *h_f2 = f2;
  T *h_f3 = f3;
  /*
    T *h_f1;
    checkCudaErrors(cudaMallocHost( (void **) &h_f1, nelem * sizeof(T) ));
    T *h_f2;
    checkCudaErrors(cudaMallocHost( (void **) &h_f2, nelem * sizeof(T) ));
    T *h_f3;
    checkCudaErrors(cudaMallocHost( (void **) &h_f3, nelem * sizeof(T) ));
  */

  // allocate device memory for input array
  T *d_f1;
  checkCudaErrors(cudaMalloc( (void **) &d_f1, nelem * sizeof(T) ));
  T *d_f2;
  checkCudaErrors(cudaMalloc( (void **) &d_f2, nelem * sizeof(T) ));
  // allocate device memory for result
  T *d_f3;
  checkCudaErrors(cudaMalloc( (void **) &d_f3,  nelem * sizeof(T) ));

  checkCudaErrors(cudaEventRecord(cu_stop, 0));
  checkCudaErrors(cudaDeviceSynchronize());                       \
  checkCudaErrors(cudaEventElapsedTime(&elapsedTimeInMs, cu_start, cu_stop));
  std::cout << "0 ->" << elapsedTimeInMs << std::endl;


  int chunks = nelem + (CHUNK_SIZE - 1) / CHUNK_SIZE;
  size_t last_chunk = nelem % (int)CHUNK_SIZE;

  //		for( int i = 0; i < chunks; i++ )
  {

    checkCudaErrors(cudaEventRecord(cu_start, 0));

    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_f1, h_f1, nelem * sizeof(T), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_f2, h_f2, nelem * sizeof(T), cudaMemcpyHostToDevice));

    checkCudaErrors(cudaEventRecord(cu_stop, 0));
    checkCudaErrors(cudaDeviceSynchronize());                       \
    checkCudaErrors(cudaEventElapsedTime(&elapsedTimeInMs, cu_start, cu_stop));
    std::cout << "1 ->" << elapsedTimeInMs << std::endl;


    checkCudaErrors(cudaEventRecord(cu_start, 0));

    // execute the kernel
    transform_GPU<<< dimGrid, dimBlock >>>(d_f1, d_f2, d_f3, nelem, bin_op);
    cudaDeviceSynchronize();

    checkCudaErrors(cudaEventRecord(cu_stop, 0));
    checkCudaErrors(cudaDeviceSynchronize());                       \
    checkCudaErrors(cudaEventElapsedTime(&elapsedTimeInMs, cu_start, cu_stop));
    std::cout << "2 ->" << elapsedTimeInMs << std::endl;



    // check if kernel execution generated and error
    //   getLastCudaError("Kernel execution failed");

    checkCudaErrors(cudaEventRecord(cu_start, 0));

    // copy result from device to host
    checkCudaErrors(cudaMemcpy(h_f3, d_f3, nelem * sizeof(T), cudaMemcpyDeviceToHost));

    checkCudaErrors(cudaEventRecord(cu_stop, 0));
    checkCudaErrors(cudaDeviceSynchronize());                       \
    checkCudaErrors(cudaEventElapsedTime(&elapsedTimeInMs, cu_start, cu_stop));
    std::cout << "3 ->" << elapsedTimeInMs << std::endl;
  }

  // free device memory
  cudaFree(d_f1);
  cudaFree(d_f2);
  cudaFree(d_f3);
}


template <typename T>
void 
transform_GPU_wrap(
		   T* h_f1, 
		   T* h_f2, 
		   T* h_f3, 
		   size_t nelem, 
		   char *op
		   )
{
  cudaDeviceProp deviceProp;
  // This will pick the best possible CUDA capable device
  int devID = findCudaDevice(0, NULL);
  checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));

  // Kernel configuration, where a one-dimensional
  // grid and one-dimensional blocks are configured.
  dim3 dimGrid(NBLOCKS);
  dim3 dimBlock(NTHREADS);

  // allocate device memory for input array
  T *d_f1;
  checkCudaErrors(cudaMalloc( (void **) &d_f1, nelem * sizeof(T) ));
  T *d_f2;
  checkCudaErrors(cudaMalloc( (void **) &d_f2, nelem * sizeof(T) ));
  // allocate device memory for result
  T *d_f3;
  checkCudaErrors(cudaMalloc( (void **) &d_f3,  nelem * sizeof(T) ));

  // copy host memory to device
  checkCudaErrors(cudaMemcpy(d_f1, h_f1, nelem * sizeof(T), cudaMemcpyHostToDevice));
  checkCudaErrors(cudaMemcpy(d_f2, h_f2, nelem * sizeof(T), cudaMemcpyHostToDevice));

  // execute the kernel
  if ( strcmp(op, "+") == 0 ) { 
    gpu_add2_I4<<< dimGrid, dimBlock >>>(d_f1, d_f2, d_f3, nelem);
  }
  else {
  }

  // check if kernel execution generated and error
  getLastCudaError("Kernel execution failed");

  // copy result from device to host
  checkCudaErrors(cudaMemcpy(h_f3, d_f3, nelem * sizeof(T), cudaMemcpyDeviceToHost));

  // free device memory
  cudaFree(d_f1);
  cudaFree(d_f2);
  cudaFree(d_f3);
}

int main(void)
{

  struct timeval start, end;

  // START: Allocate and initialize code 
  myType* f1 = (myType*) malloc(NELEM * sizeof(myType));
  myType* f2 = (myType*) malloc(NELEM * sizeof(myType));
  myType* f3 = (myType*) malloc(NELEM * sizeof(myType));

  for ( long long i=0; i<NELEM; i++) {
    f1[i] = i;
    f2[i] = NELEM - i - 1;
  }
  // STOP: Allocate and initialize code 

  // VARIANT 1: THRUST

  gettimeofday(&start, NULL);
  //	transform_thrust(f1, f2, f3, NELEM, f1f2_functor<myType>(0));
  transform_GPU_wrap(f1, f2, f3, NELEM, f1f2_functor<myType>(0));
  transform_GPU_streams_wrap(f1, f2, f3, NELEM, f1f2_functor<myType>(0));
  gettimeofday(&end, NULL);
  std::cout << "gpu add = " << getTimeDiff(start, end) << "ms" << std::endl;


  //_CILK_FOR equivalent

  /*
    TIMETHIS( for(long long i=0; i<(long long)(NELEM); i++, y[i] = 2.0 * x[i] + y[i]), &start, &end );

    cout << "cpu add = " << getTimeDiff(start, end) << endl;
  */

  //SEQUENTIAL

  thrust::host_vector<myType> h_f1 (f1, f1 + (size_t)NELEM);
  thrust::host_vector<myType> h_f2 (f2, f2 + (size_t)NELEM);
  thrust::host_vector<myType> h_f3_cpu(NELEM);
  //	TIMETHIS( std::transform (h_f1.begin(), h_f1.end(), h_f2.begin(), h_f3_cpu.begin(), f1f2_functor<myType>()), &start, &end );

  //	std::cout << "cpu add = " << getTimeDiff(start, end) << "ms" << std::endl;


  TIMETHIS( for(long long i=0; i<NELEM; h_f3_cpu[i] = f1[i] + f2[i], i++), &start, &end );

  std::cout << "cpu add = " << getTimeDiff(start, end) << "ms" << std::endl;

  for(long long i=0; i < NELEM; i++)
    {
      if(f3[i] != h_f3_cpu[i])
	{
	  std::cout << "Results dont match!!! at " << i << " cpu: " << h_f3_cpu[i] << " gpu: " << f3[i] << std::endl;
	  break;
	}
    }

  free(f1);
  free(f2);
  free(f3);

  return 0;
}
