#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "hash_string.h"

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  int itemp;
  char *hashtype = NULL;
  unsigned long long ltemp;

  if ( argc != 3 ) { fprintf(stderr, "Usage is %s <word to hash> <I|LL> \n", argv[0]); go_BYE(-1); }
  hashtype = argv[2];
  if ( strcmp(hashtype, "I") == 0 )  {
    status = hash_string(argv[1], &itemp);
    cBYE(status);
    fprintf(stdout, "%d,%s \n", itemp, argv[1]);
  }
  else if ( strcmp(hashtype, "LL") == 0 )  {
    status = ll_hash_string(argv[1], &ltemp);
    cBYE(status);
    fprintf(stdout, "%lld,%s \n", ltemp, argv[1]);
  }
  else { go_BYE(-1); }
 BYE:
  return(status);
}
