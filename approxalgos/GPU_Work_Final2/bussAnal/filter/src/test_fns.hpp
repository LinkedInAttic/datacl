#include <iostream>

#include "filter.hpp"
#include "filter.cuh"
#include "common.hpp"
using namespace std;


/**
* @brief testing function to test and time filter (serial and GPU version) with random inputs.
*/
template <typename T>
void testfilter(size_t nelem)
{
    // timers
    struct timeval start, end;

    // data array
    T *h_data = (T *) malloc(nelem * sizeof(T));
    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(nelem * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(nelem * sizeof(char));

    // initalize the memory
    data_generate( h_data, nelem, (T)0, (T)RAND_MAX );
    T val = rand();

    // declare functor for comparator callback function
    opFunctor<T> opFuncNew;
    int opcode = rand() % 5;//    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterSimpleGPU_wrap <T> ( h_data, nelem, val, h_bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filter<T> (h_data, (size_t)nelem, val, bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, nelem );

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}

/**
* @brief testing function to test and time filter (serial and GPU version with pinned memory with cudaHostRegister()) with random inputs.
*/
template <typename T>
void testfilter_PinnedRegMem(size_t nelem)
{
    // timers
    struct timeval start, end;

    // data array
    T *h_data = (T *) malloc(nelem * sizeof(T));
    // allocate mem for the gpu result (pinned mem) on host side
    char *h_bitvec_pin = (char *) malloc(nelem * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(nelem * sizeof(char));


    // initalize the memory
    data_generate( h_data, nelem, (T)0, (T)RAND_MAX );

    T val = rand();

    // declare functor for comparator callback function
    opFunctor<T> opFuncNew;
    int opcode = rand() % 5; //    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterGPU_wrap_regPin <T> ( h_data, nelem, val, h_bitvec_pin, opFuncNew), &start, &end );

    cout << "Time Taken by GPU (pinned mem using cudaHostRegister): " << getTimeDiff(start, end) << "ms" << endl;




    // run on host for comparison
    TIMETHIS( filter<T> (h_data, (size_t)nelem, val, bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec_pin, bitvec, nelem );

    // free host memory
    free(h_data);
    free(h_bitvec_pin);
    free(bitvec);
}

/**
* @brief testing function to test and time filter (serial and GPU version with pinned memory with cudaHostAlloc()) with random inputs.
*/
template <typename T>
void testfilter_PinnedMem(size_t nelem)
{
    // timers
    struct timeval start, end;

    T *h_data_p;
    char* h_bitvec_p;

    checkCudaErrors(cudaHostAlloc((void **)&h_data_p, nelem * sizeof(T), cudaHostAllocMapped));
    // allocate mem for the gpu result on host side
    checkCudaErrors(cudaHostAlloc((void **)&h_bitvec_p, nelem * sizeof(char), cudaHostAllocMapped));
    // allocate mem for cpu result array for verification
    char* bitvec = (char *) malloc(nelem * sizeof(char));

    // initalize the memory
    data_generate( h_data_p, nelem, (T)0, (T)RAND_MAX);

    T val = rand();

    // declare functor for comparator callback function
    opFunctor<T> opFuncNew;
    int opcode = rand() % 5;//    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterGPU_wrap_pin <T> ( h_data_p, nelem, val, h_bitvec_p, opFuncNew), &start, &end );

    cout << "Time Taken by GPU (pinned mem using cudaHostAlloc()): " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filter<T> (h_data_p, (size_t)nelem, val, bitvec, opFuncNew), &start, &end );


    verifyResult<char>( h_bitvec_p, bitvec, nelem );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    // free host memory
    free(bitvec);

    //free device memory
    checkCudaErrors(cudaFreeHost(h_data_p));
    checkCudaErrors(cudaFreeHost(h_bitvec_p));
}

/**
* @brief testing function for filter in.
*/
template <typename T>
void testfilterIn(size_t nelem, size_t ndict)
{
    // timers
    struct timeval start, end;

    // data array
    T *h_data = (T *) malloc(nelem * sizeof(T));
    // data array
    T *h_dict = (T *) malloc(ndict * sizeof(T));
    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(nelem * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(nelem * sizeof(char));

    // initalize the memory
    data_generate( h_data, nelem, (T)0, (T)nelem );

    // initalize the dictionary
    data_generate( h_dict, ndict, (T)0, (T)nelem );

    cout << "Checking for elements of input array in dictionary (using global memory of GPU to store the dictionary) " << endl;

    // run on gpu
    TIMETHIS( filterInGPU_wrap <T> ( h_data, nelem, h_dict, ndict, h_bitvec ), &start, &end );

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filterIn<T> ( h_data, nelem, h_dict, ndict, bitvec ), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, nelem );

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}

/**
* @brief testing function for filter in - this variant uses constant memory of the GPU which is a faster read-only memory.
*/
template <typename T>
void testfilterIn_conMem(size_t nelem, size_t ndict)
{
    // timers
    struct timeval start, end;

    // data array
    T *h_data = (T *) malloc(nelem * sizeof(T));
    // data array
    T *h_dict = (T *) malloc(ndict * sizeof(T));

    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(nelem * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(nelem * sizeof(char));

    // initalize the memory
    data_generate( h_data, nelem, (T)0, (T)nelem );

    // initalize the dictionary
    data_generate( h_dict, ndict, (T)0, (T)nelem );

    gettimeofday(&start, NULL);
    // allocate input and output data arrays
    T *d_data;
    checkCudaErrors(cudaMalloc( (void **) &d_data, nelem * sizeof(T) ));
    char *d_bitvec;
    checkCudaErrors(cudaMalloc( (void **) &d_bitvec,  nelem * sizeof(char) ));

    // copy data array to device
    checkCudaErrors(cudaMemcpy(d_data, h_data, nelem * sizeof(T), cudaMemcpyHostToDevice));

    cout << "Checking for elements of input array in dictionary (using constant memory of GPU to store the dictionary) " << endl;

    // run on gpu
    filterInGPU_conMem_wrap <T> ( d_data, nelem, h_dict, ndict, d_bitvec );

    checkCudaErrors(cudaMemcpy(h_bitvec, d_bitvec, nelem * sizeof(char), cudaMemcpyDeviceToHost));
    gettimeofday(&end, NULL);

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filterIn<T> ( h_data, nelem, h_dict, ndict, bitvec ), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, nelem );

    // free device memory
    cudaFree(d_data);
    cudaFree(d_bitvec);

    cudaDeviceReset();

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}
