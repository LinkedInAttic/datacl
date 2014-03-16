#include <sys/time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

double getTimeDiff( struct timeval& start, struct timeval& end )
{
		return ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)) / 1000 ;
}
