#include <stdio.h>
#include <time.h>

// START FUNC DECL
int 
time1(
    int *ptr_t
    )
// STOP FUNC DECL
{
  int status = 0;
  *ptr_t = time(NULL);
  return(status);
}
