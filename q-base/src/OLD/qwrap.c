#include "qtypes.h"
#include "mmap.h"
#include "q.h"

#define RSLT_BUF_SIZE 4096
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char rslt_buf[RSLT_BUF_SIZE];

  zero_string(rslt_buf, RSLT_BUF_SIZE);
  status = q(argc, argv, rslt_buf, RSLT_BUF_SIZE);
  cBYE(status);
  if ( *rslt_buf != '\0' ) {
    fprintf(stdout, "%s", rslt_buf);
  }
 BYE:
  return(status);
}

