#include <stdio.h>
#include "constants.h"
#include "macros.h"

int
main()
{
  long long t1, t2;
  asm("RDTSC\n\t"
      "mov DWORD PTR t1\n\t"
      "mov DWORD PTR t1+4\n\t"
     );
}
