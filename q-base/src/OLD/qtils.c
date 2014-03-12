#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "time1.h"
#include "replace_char.h"
 
// START FUNC DECL
int
qtils(
     int argc,
     char **argv,
     char *rslt_buf,
     int sz_rslt_buf
     )
// STOP FUNC DECL
{
  int status = 0;
  int itemp;
  if ( ( argc <= 1 ) || ( argc >= 6 ) )  {go_BYE(-1); }
  if ( strcmp(argv[1], "curr_time_sec_since_epoch") == 0 ) { 
    if ( argc != 2 ) { go_BYE(-1); }
    status = time1(&itemp);
    sprintf(rslt_buf, "%d", itemp);
  }
  else if ( strcmp(argv[1], "replace_char") == 0 ) { 
    if ( argc != 5 ) { go_BYE(-1); }
    status = replace_char(argv[2], argv[3], argv[4]);
    sprintf(rslt_buf, "%d", itemp);
  }
  else { go_BYE(-1); }
BYE:
  return(status);
}
