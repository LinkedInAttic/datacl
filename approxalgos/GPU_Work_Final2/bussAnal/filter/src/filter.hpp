/**
* @brief filter function.
*
* @tparam T template argument
* @tparam Compare comparator
* @param data input array
* @param numElem size of input array
* @param val value to be compared with
* @param outFlag char array containing results of comparison (1=true, 0=false)
* @param comp comparator
*/
template <class T, class Compare>
void filter(T data[], size_t numElem, const T val, char bitvec[], Compare comp)
{
	for(unsigned long long i=0; i<numElem; i++)
	{
		if( comp(data[i], val) )
			bitvec[i] = '1';
		else
			bitvec[i] = '0';
	}
}


/**
* @brief binary search
*
* @tparam T template argument
* @param a[] array to be searched on
* @param n number of elements
* @param target target to be searched
*
* @return index of element if found, -1 if not found
*/
template<typename T>
int binarySearch(T a[], size_t n, T target) {

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
* @brief function that does filter in - search for the existence of each element of input array and return 1(true) or 0(false)
*
* @tparam T template argument
* @param inData input data array
* @param numElem number of elements in data array
* @param inDict input dictionary array
* @param numElemInDict number of elements in dictionary
* @param outFlag flag array which should be written to with 0(false) or 1(true)
*/
template <typename T>
void filterIn (T *inData, size_t numElem, T* inDict, size_t numElemInDict, char* outFlag)
{
	for(unsigned long long i=0; i<numElem; i++)
	{
		if( binarySearch <T> (inDict, numElemInDict, inData[i]) == -1 )
			outFlag[i] = '0';
		else
			outFlag[i] = '1';
	}
}
