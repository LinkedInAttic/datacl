#include <stdio.h>
#include "spooky_hash.h"
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

int main()
{

  int status = 0;
  uint64_t seed = time(NULL);

  for ( int ii = -15; ii <= 15 ; ii++ ) {

    uint64_t hashval;
    int jj = ii/2;
    int * t_value = &jj;
    
    hashval = spooky_hash64((void *)t_value, sizeof(int), seed);

    printf("\n%"PRIu64, hashval);

  }

  return (status);

}
