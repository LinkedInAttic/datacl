#define TIMETHIS(command, startTimer, endTimer)	\
gettimeofday(startTimer, NULL);			\
command;					\
gettimeofday(endTimer, NULL);			\

/**
* @brief get time difference between timers star and end
*
* @param start start timer - has to be struct timeval pointer
* @param end end timer - has to be struct timeval pointer
*
* @return time difference in millisecond
*/
double getTimeDiff( struct timeval& start, struct timeval& end );


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
										std::cout << "Results dont match!!! at " << i << " gpu: " << arrGPU[i] << " cpu: " << arrCPU[i] << std::endl;
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
    datavec[i] = low + (high - low) * ((double) rand() / (double) RAND_MAX);
}
