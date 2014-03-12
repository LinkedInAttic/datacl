#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "../fastbit/fastbit.h"

extern char *g_data_dir;

// START FUNC DECL
int fb_init(
    )
// STOP FUNC DECL
{
  int status = 0;

  if ( g_data_dir == NULL ) { return(-1); }
  // TODO 
BYE:
  return(status);
}
