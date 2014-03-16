#include <helper_cuda.h>
#include <helper_functions.h> // helper functions for SDK examples

#define NTHREADS 512
#define NBLOCKS 512
#define CMEM_MAXSIZE 8000

__constant__ long long constDict[6000];

/**
* @brief enum for different types of comparison operations
*/
enum op_t {EQ, LT, LE, GT, GE, NE};


/**
* @brief functor for different comparators
*
* @tparam T template parameter
*/
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

/**
* @brief binary search on device
*
* @tparam T template argument
* @param a[] array to be searched on
* @param n number of elements
* @param target target value to be searched
*
* @return index of element if found, -1 if not found
*/
template<typename T>
__device__ int binarySearchGPU(T a[], long long n, T target) {

    long long low = 0;
    long long high = n-1;

    while (low <= high) {
        int middle = low + (high - low)/2;
	if (target < a[middle])
            high = middle - 1;
        else if (a[middle] < target)
            low = middle + 1;
        else
            return middle;
    }
    return -1;
}


/**
* @brief binary search on device - uses constant memory, assumes the dict is in constant memory array named "constDict"
*
* @tparam T template argument
* @param a[] array to be searched on
* @param n number of elements
* @param target target value to be searched
*
* @return index of element if found, -1 if not found
*/
template<typename T>
__device__ int binarySearchGPU_conMem(long long n, T target) {

    long long low = 0;
    long long high = n-1;

    while (low <= high) {
        int middle = low + (high - low)/2;
	if (target < constDict[middle])
            high = middle - 1;
        else if (constDict[middle] < target)
            low = middle + 1;
        else
            return middle;
    }
    return -1;
}

/**
* @brief kernel for filter in - search for the existence of each element of input array in a given dictionary array and return 1(true) or 0(false)
*
* @tparam T template argument
* @param inData input data array
* @param numElem number of elements in data array
* @param inDict input dictionary array
* @param numElemInDict number of elements in dictionary
* @param outFlag flag array which should be written to with 0(false) or 1(true)
*/
template <typename T>
__global__ void filterInGPU (T *inData, size_t numElem, T* inDict, size_t numElemInDict, char* outFlag)
{
        long long tid = blockDim.x * blockIdx.x + threadIdx.x;

        while(tid < numElem)
        {
                if( binarySearchGPU <T> (inDict, numElemInDict, inData[tid]) == -1 )
                        outFlag[tid] = '0';
                else
                        outFlag[tid] = '1';
                tid += blockDim.x * gridDim.x;
        }
}


/**
* @brief kernel for filter in - search for the existence of each element of input array in a given dictionary array and return 1(true) or 0(false) - uses constant memory, assumes the dict is in constant memory array named "constDict"
*
* @tparam T template argument
* @param inData input data array
* @param numElem number of elements in data array
* @param inDict input dictionary array
* @param numElemInDict number of elements in dictionary
* @param outFlag flag array which should be written to with 0(false) or 1(true)
*/
template <typename T>
__global__ void filterInGPU_conMem (T *inData, size_t numElem, size_t numElemInDict, char* outFlag)
{
        long long tid = blockDim.x * blockIdx.x + threadIdx.x;

        while(tid < numElem)
        {
                if( binarySearchGPU_conMem <T> (numElemInDict, inData[tid]) == -1 )
                        outFlag[tid] = '0';
                else
                        outFlag[tid] = '1';
                tid += blockDim.x * gridDim.x;
        }
}


/**
* @brief filter GPU kernel.
*
* @tparam T template argument
* @tparam Compare comparator
* @param data input array
* @param numElem size of input array
* @param val value to be compared with
* @param outFlag char array containing results of comparison (1=true, 0=false)
* @param comp comparator
*/
template <typename T, class Compare>
__global__ void filterGPU(T *data, size_t numElem, const T val, char *outFlag, Compare comp)
{
        //1-D thread index
        long long tid = blockDim.x * blockIdx.x + threadIdx.x;

        while(tid < numElem)
        {
                if( comp(data[tid], val) )
                        outFlag[tid] = '1';
                else
                        outFlag[tid] = '0';
                tid += blockDim.x * gridDim.x;
        }
}


/**
* @brief function that determines max amount of global memory available on the GPU. This function is not being used anywhere but just keeping in case it might be useful later. Shamelessly lifted from :http://stackoverflow.com/questions/8905949/why-is-cudamalloc-giving-me-an-error-when-i-know-there-is-sufficient-memory-spac
*
* @tparam T template parameter
*
* @return max available global memory on GPU
*/
template <typename T>
size_t getMaxAvailGpuMem()
{

        const size_t Mb = 1<<20; // Assuming a 1Mb page size here

        size_t available, total;
        cudaMemGetInfo(&available, &total);

        T *buf_d = 0;
        size_t nwords = available / sizeof(T);
        size_t words_per_Mb = Mb / sizeof(T);

        while(cudaMalloc((void**)&buf_d,  nwords * sizeof(T)) == cudaErrorMemoryAllocation)
        {
                nwords -= words_per_Mb;
                if( nwords  < words_per_Mb)
                {
                        // signal no free memory
                        break;
                }
        }

        cudaDeviceReset();
        return nwords;

}



/**
* @brief wrapper function which calls the GPU to check if the elements of a data array satisfy the given comparator and value and returns char array with 1s (true) or 0s(false) based on the filtering.
*
* @tparam T data template argument
* @tparam Compare comparator template argument
* @param h_data[] input data in host memory
* @param numElem number of elements
* @param val value to be checked
* @param h_bitvec[] output char array
* @param comp comparator
*/
template <class T, class Compare>
void filterGPU_wrap(T h_data[], size_t numElem, const T val, char h_bitvec[], Compare comp)
{
    cudaDeviceProp deviceProp;

    // This will pick the best possible CUDA capable device
    int devID = findCudaDevice(0, NULL);

    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));

    // Kernel configuration, where a one-dimensional
    // grid and one-dimensional blocks are configured.
    dim3 dimGrid(NBLOCKS);
    dim3 dimBlock(NTHREADS);

    size_t available, total;
    // Get total and available global memory on GPU
    cudaMemGetInfo(&available, &total);

    // If size of data wont fit in available global memory, it has to be batch-processed. Determine max number of elements that should be sent into the gpu in one iteration. only a portion of the gpu global memory (half currently) is used since not the entire amount can be used apparently (further reading reqd to find out how much is the exact max amount that can be allocated)
    long long maxNumElem = available / sizeof(T) /2;
    // find corresponding number of iterations reqd to process the entire data
    long long numiter = ( numElem + (maxNumElem -1) ) / maxNumElem;

    // allocate device memory
    T *d_data;
    checkCudaErrors(cudaMalloc((void **) &d_data, MIN(maxNumElem, numElem) * sizeof(T) ));

    // allocate device memory for result
    char *d_bitvec;
    checkCudaErrors(cudaMalloc((void **) &d_bitvec, MIN(maxNumElem, numElem) * sizeof(char)));

    long long dataRem = numElem;
    for(int i=0; i<numiter; i++)
    {
            // data offset
            long long offset = i * maxNumElem;
            // number of elements to be processed in this iteration
            long long currDataSize = MIN(dataRem, maxNumElem);

            // copy host memory to device
            checkCudaErrors(cudaMemcpy(d_data, (h_data + offset), currDataSize * sizeof(T), cudaMemcpyHostToDevice));

            // execute the kernel
            filterGPU<<< dimGrid, dimBlock >>>(d_data, currDataSize, val, d_bitvec, comp);

            cudaDeviceSynchronize();

            // check if kernel execution generated and error
            getLastCudaError("Kernel execution failed");

            // copy result from device to host
            checkCudaErrors(cudaMemcpy((h_bitvec + offset), d_bitvec, currDataSize * sizeof(char), cudaMemcpyDeviceToHost));

            // number of elements not yet processed
            dataRem = dataRem - currDataSize;

    }

    // free device memory
    cudaFree(d_data);
    cudaFree(d_bitvec);

    cudaDeviceReset();
}



/**
* @brief wrapper function which calls the GPU to check if the elements of a data array satisfy the given comparator and value and returns char array with 1s (true) or 0s(false) based on the filtering. This variant uses page-locked host memory (it assumes the input array was allocated using cudaHostAlloc() which allocates page-locked host memory) which according to the manual should be faster: http://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#page-locked-host-memory
*
* @tparam T data template argument
* @tparam Compare comparator template argument
* @param h_data[] input data in host memory
* @param numElem number of elements
* @param val value to be checked
* @param h_bitvec[] output char array
* @param comp comparator
*/
template <class T, class Compare>
void filterGPU_wrap_pin(T h_data[], size_t numElem, const T val, char h_bitvec[], Compare comp)
{

    cudaDeviceProp deviceProp;

    // This will pick the best possible CUDA capable device
    int devID = findCudaDevice(0, NULL);

    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));

    // page-lock a data array memory allocated by malloc()
    T *d_data;
    checkCudaErrors(cudaHostGetDevicePointer((void **)&d_data, (void *)h_data, 0));

    // page-lock a output char array memory allocated by malloc()
    char *d_bitvec;
    checkCudaErrors(cudaHostGetDevicePointer((void **)&d_bitvec, (void *)h_bitvec, 0));

    // Kernel configuration, where a one-dimensional
    // grid and one-dimensional blocks are configured.
    dim3 dimGrid(NBLOCKS);
    dim3 dimBlock(NTHREADS);

    // execute the kernel
    filterGPU<<< dimGrid, dimBlock >>>(d_data, numElem, val, d_bitvec, comp);

    cudaThreadSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");
}



/**
* @brief wrapper function which calls the GPU to check if the elements of a data array satisfy the given comparator and value and returns char array with 1s (true) or 0s(false) based on the filtering. This variant uses page-locked host memory (cudaHostRegister() page-locks a range of memory allocated by malloc()) which according to the manual should be faster: http://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#page-locked-host-memory
*
* @tparam T data template argument
* @tparam Compare comparator template argument
* @param h_data[] input data in host memory
* @param numElem number of elements
* @param val value to be checked
* @param h_bitvec[] output char array
* @param comp comparator
*/
template <class T, class Compare>
void filterGPU_wrap_regPin(T h_data[], size_t numElem, const T val, char h_bitvec[], Compare comp)
{
    cudaDeviceProp deviceProp;

    // This will pick the best possible CUDA capable device
    int devID = findCudaDevice(0, NULL);
    checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));

    T *d_data;
    char *d_bitvec;

    checkCudaErrors(cudaHostRegister(h_data, numElem * sizeof(T), cudaHostRegisterMapped));
    checkCudaErrors(cudaHostRegister(h_bitvec, numElem * sizeof(char), cudaHostRegisterMapped));

    checkCudaErrors(cudaHostGetDevicePointer((void **)&d_data, (void *)h_data, 0));
    checkCudaErrors(cudaHostGetDevicePointer((void **)&d_bitvec, (void *)h_bitvec, 0));


    // Kernel configuration, where a one-dimensional
    // grid and one-dimensional blocks are configured.
    dim3 dimGrid(NBLOCKS);
    dim3 dimBlock(NTHREADS);

    // execute the kernel
    filterGPU<<< dimGrid, dimBlock >>>(d_data, numElem, val, d_bitvec, comp);

    cudaThreadSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");

    checkCudaErrors(cudaHostUnregister(h_data));
    checkCudaErrors(cudaHostUnregister(h_bitvec));

    cudaDeviceReset();

}

/**
* @brief wrapper for filter in - search for the existence of each element of input array in the given dictionary array and return 1(true) or 0(false)
*
* @tparam T template argument
* @param h_data[] input data array
* @param numElem number of elements in data array
* @param h_dict[] input dictionary array 
* @param numElemDict number of elements in dictionary
* @param h_bitvec[] flag array which should be written to with 0(false) or 1(true)
*/
template <class T>
void filterInGPU_wrap(T h_data[], size_t numElem, T h_dict[], size_t numElemDict, char h_bitvec[])
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
    T *d_data;
    checkCudaErrors(cudaMalloc( (void **) &d_data, numElem * sizeof(T) ));
    // allocate device memory for dictionary
    T *d_dict;
    checkCudaErrors(cudaMalloc( (void **) &d_dict, numElemDict * sizeof(T) ));
    // allocate device memory for result
    char *d_bitvec;
    checkCudaErrors(cudaMalloc( (void **) &d_bitvec,  numElem * sizeof(char) ));

    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_data, h_data, numElem * sizeof(T), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_dict, h_dict, numElemDict * sizeof(T), cudaMemcpyHostToDevice));

    // execute the kernel
    filterInGPU<<< dimGrid, dimBlock >>>(d_data, numElem, d_dict, numElemDict, d_bitvec);

    cudaDeviceSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");

    // copy result from device to host
    checkCudaErrors(cudaMemcpy(h_bitvec, d_bitvec, numElem * sizeof(char), cudaMemcpyDeviceToHost));

    // free device memory
    cudaFree(d_data);
    cudaFree(d_dict);
    cudaFree(d_bitvec);

    cudaDeviceReset();
}


/**
* @brief wrapper for filter in (this variant uses the constant memory of the GPU, and assumes a constant memory array of sufficient size has been declared to store the dict array) - search for the existence of each element of input array in a given dictionary array and return 1(true) or 0(false)
*
* @tparam T template argument
* @param d_data[] input data array (device array)
* @param numElem number of elements in data array
* @param h_dict[] input dictionary array 
* @param numElemDict number of elements in dictionary
* @param d_bitvec[] flag array (device array) which should be written to with 0(false) or 1(true)
*/
template <class T>
void filterInGPU_conMem_wrap(T d_data[], size_t numElem, T h_dict[], size_t numElemDict, char d_bitvec[])
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
    checkCudaErrors(cudaMemcpyToSymbol (constDict, h_dict, numElemDict * sizeof(T) ));

    // execute the kernel
    filterInGPU_conMem<<< dimGrid, dimBlock >>>(d_data, numElem, numElemDict, d_bitvec);

    cudaDeviceSynchronize();

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");
}

