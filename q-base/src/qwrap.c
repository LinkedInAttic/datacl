/* DO NOT AUTO GENERATE HEADER FILE  */
#include "qtypes.h"
#include "auxil.h"
#include "q.h"
#include "q_batch.h"

#define RSLT_BUF_SIZE 4096
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char rslt_buf[RSLT_BUF_SIZE];
  FILE *fp = NULL;

  zero_string(rslt_buf, RSLT_BUF_SIZE);

  if ( ( argc == 3 ) && ( strcmp(argv[1], "batch") == 0 ) ) {
    status = q_batch(argc, argv, rslt_buf, RSLT_BUF_SIZE, stdout);
  }
  else {
    status = q(argc, argv, rslt_buf, RSLT_BUF_SIZE, stdout);
    cBYE(status);
  }
  if ( *rslt_buf != '\0' ) {
    fprintf(stdout, "%s", rslt_buf);
  }
 BYE:
  fclose_if_non_null(fp);
  return(status);
}

