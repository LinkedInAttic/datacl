#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda_runtime.h>
#include <string>

#include "filter.hpp"
#include "filter.cuh"
using namespace std;

#define NELEM 5e8
#define NDICT 5000

#define TIMETHIS(command, startTimer, endTimer)	\
gettimeofday(startTimer, NULL);			\
command;					\
gettimeofday(endTimer, NULL);			\



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


/**
* @brief verifies if the given arrays are identical up to dataSize elements
*
* @param arrGPU[] GPU result
* @param arrCPU[] CPU result
* @param dataSize number of elements to compare
*/
template <typename T>
void verifyResult( T arrGPU[], T arrCPU[], size_t dataSize )
{ 
    for(long long i=0; i < dataSize; i++)
    {
            if(arrGPU[i] != arrCPU[i])
            {
                    cout << "Results dont match!!! at " << i << " gpu: " << arrGPU[i] << " cpu: " << arrCPU[i] << endl;
                    break;
            }
    }
}

/**
* @brief generates random data
*
* @tparam T template parameter
* @param datavec data array
* @param dataSize number of elements
*/
template <typename T>
void data_generate(T* datavec, size_t dataSize, T low, T high)
{
	for( long long i=0; i<dataSize; i++ )
		//generating between 0 and RAND_MAX
		datavec[i] = low + (high - low) * ((double) rand() / (double) RAND_MAX);
}





/**
* @brief testing function to test and time filter (serial and GPU version) with random inputs.
*/
void testfilter()
{
    // timers
    struct timeval start, end;

    // data array
    myType *h_data = (myType *) malloc(NELEM * sizeof(myType));
    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(NELEM * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(NELEM * sizeof(char));

    // initalize the memory
    data_generate( h_data, NELEM, (myType)0, (myType)RAND_MAX );
    myType val = rand();

    // declare functor for comparator callback function
    opFunctor<myType> opFuncNew;
    int opcode = rand() % 5;//    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterGPU_wrap <myType> ( h_data, NELEM, val, h_bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filter<myType> (h_data, (size_t)NELEM, val, bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, NELEM );

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}



/**
* @brief testing function to test and time filter (serial and GPU version with pinned memory with cudaHostRegister()) with random inputs.
*/
void testfilter_PinnedRegMem()
{
    // timers
    struct timeval start, end;

    // data array
    myType *h_data = (myType *) malloc(NELEM * sizeof(myType));
    // allocate mem for the gpu result (pinned mem) on host side
    char *h_bitvec_pin = (char *) malloc(NELEM * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(NELEM * sizeof(char));


    // initalize the memory
    data_generate( h_data, NELEM, (myType)0, (myType)RAND_MAX );

    myType val = rand();

    // declare functor for comparator callback function
    opFunctor<myType> opFuncNew;
    int opcode = rand() % 5; //    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterGPU_wrap_regPin <myType> ( h_data, NELEM, val, h_bitvec_pin, opFuncNew), &start, &end );

    cout << "Time Taken by GPU (pinned mem using cudaHostRegister): " << getTimeDiff(start, end) << "ms" << endl;




    // run on host for comparison
    TIMETHIS( filter<myType> (h_data, (size_t)NELEM, val, bitvec, opFuncNew), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec_pin, bitvec, NELEM );

    // free host memory
    free(h_data);
    free(h_bitvec_pin);
    free(bitvec);
}





/**
* @brief testing function to test and time filter (serial and GPU version with pinned memory with cudaHostAlloc()) with random inputs.
*/
void testfilter_PinnedMem()
{
    // timers
    struct timeval start, end;

    myType *h_data_p;
    char* h_bitvec_p;

    checkCudaErrors(cudaHostAlloc((void **)&h_data_p, NELEM * sizeof(myType), cudaHostAllocMapped));
    // allocate mem for the gpu result on host side
    checkCudaErrors(cudaHostAlloc((void **)&h_bitvec_p, NELEM * sizeof(char), cudaHostAllocMapped));
    // allocate mem for cpu result array for verification
    char* bitvec = (char *) malloc(NELEM * sizeof(char));

    // initalize the memory
    data_generate( h_data_p, NELEM, (myType)0, (myType)RAND_MAX);

    myType val = rand();

    // declare functor for comparator callback function
    opFunctor<myType> opFuncNew;
    int opcode = rand() % 5;//    op_t opNew = GT;
    opFuncNew.setOp(opcode);

    cout << "Checking for value " << val << " and opcode " << opcode << endl;

    // run on gpu
    TIMETHIS( filterGPU_wrap_pin <myType> ( h_data_p, NELEM, val, h_bitvec_p, opFuncNew), &start, &end );

    cout << "Time Taken by GPU (pinned mem using cudaHostAlloc()): " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filter<myType> (h_data_p, (size_t)NELEM, val, bitvec, opFuncNew), &start, &end );


    verifyResult<char>( h_bitvec_p, bitvec, NELEM );

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
void testfilterIn()
{
    // timers
    struct timeval start, end;

    // data array
    myType *h_data = (myType *) malloc(NELEM * sizeof(myType));
    // data array
    myType *h_dict = (myType *) malloc(NDICT * sizeof(myType));
    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(NELEM * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(NELEM * sizeof(char));

    // initalize the memory
    data_generate( h_data, NELEM, (myType)0, (myType)NELEM );

    // initalize the dictionary
    data_generate( h_dict, NDICT, (myType)0, (myType)NELEM );

    cout << "Checking for elements of input array in dictionary (using global memory of GPU to store the dictionary) " << endl;

    // run on gpu
    TIMETHIS( filterInGPU_wrap <myType> ( h_data, NELEM, h_dict, NDICT, h_bitvec ), &start, &end );

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filterIn<myType> ( h_data, NELEM, h_dict, NDICT, bitvec ), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, NELEM );

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}


/**
* @brief testing function for filter in - this variant uses constant memory of the GPU which is a faster read-only memory.
*/
void testfilterIn_conMem()
{
    // timers
    struct timeval start, end;

    // data array
    myType *h_data = (myType *) malloc(NELEM * sizeof(myType));
    // data array
    myType *h_dict = (myType *) malloc(NDICT * sizeof(myType));

    // allocate mem for the gpu result on host side
    char *h_bitvec = (char *) malloc(NELEM * sizeof(char));
    // allocate mem for cpu result array for verification
    char *bitvec = (char *) malloc(NELEM * sizeof(char));

    // initalize the memory
    data_generate( h_data, NELEM, (myType)0, (myType)NELEM );

    // initalize the dictionary
    data_generate( h_dict, NDICT, (myType)0, (myType)NELEM );

    gettimeofday(&start, NULL);
    // allocate input and output data arrays
    myType *d_data;
    checkCudaErrors(cudaMalloc( (void **) &d_data, NELEM * sizeof(myType) ));
    char *d_bitvec;
    checkCudaErrors(cudaMalloc( (void **) &d_bitvec,  NELEM * sizeof(char) ));

    // copy data array to device
    checkCudaErrors(cudaMemcpy(d_data, h_data, NELEM * sizeof(myType), cudaMemcpyHostToDevice));

    cout << "Checking for elements of input array in dictionary (using constant memory of GPU to store the dictionary) " << endl;

    // run on gpu
    filterInGPU_conMem_wrap <myType> ( d_data, NELEM, h_dict, NDICT, d_bitvec );

    checkCudaErrors(cudaMemcpy(h_bitvec, d_bitvec, NELEM * sizeof(char), cudaMemcpyDeviceToHost));
    gettimeofday(&end, NULL);

    cout << "Time Taken by GPU: " << getTimeDiff(start, end) << "ms" << endl;

    // run on host for comparison
    TIMETHIS( filterIn<myType> ( h_data, NELEM, h_dict, NDICT, bitvec ), &start, &end );

    cout << "Time Taken by CPU: " << getTimeDiff(start, end) << "ms" << endl;

    verifyResult<char>( h_bitvec, bitvec, NELEM );

    // free device memory
    cudaFree(d_data);
    cudaFree(d_bitvec);

    cudaDeviceReset();

    // free host memory
    free(h_data);
    free(h_bitvec);
    free(bitvec);
}




int main(int argc, char **argv)
{


srand( time(NULL) );
/*
cout << "-----------------------------------------------------" << endl;

testfilter();

cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;

testfilter_PinnedRegMem();


cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;

testfilter_PinnedMem();

cout << endl;
cout << endl;
cout << "-----------------------------------------------------" << endl;
*/

cout << "-----------------------------------------------------" << endl;
cout << endl;

testfilterIn();

cout << endl;
cout << "-----------------------------------------------------" << endl;


cout << "-----------------------------------------------------" << endl;
cout << endl;

testfilterIn_conMem();

cout << endl;
cout << "-----------------------------------------------------" << endl;

    return 0;
}
