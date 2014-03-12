#include <stdio.h>
#include <time.h>

int time1(
    int *ptr_t
    )
{
  int status = 0;
  *ptr_t = time(NULL);
  return(status);
}
