#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sort_1_long_long.h"

int sort_1_a_long_long_compare(
    const void *vi,
    const void *vj
    )
{ 
  long long val_i, val_j;
  long long *val_ptr_i, *val_ptr_j;

  val_ptr_i = (long long *)vi;
  val_ptr_j = (long long *)vj;
  val_i = *val_ptr_i;
  val_j = *val_ptr_j;

  /* Output in ascending order */
  if ( val_i > val_j )  {
    return (1);
  }
  else if ( val_i < val_j ) {
    return (-1);
  }
  else {
    return(0);
  }
}
//-------------------------------------------
int sort_1_d_long_long_compare(
    const void *vi,
    const void *vj
    )
{ 
  long long val_i, val_j;
  long long *val_ptr_i, *val_ptr_j;

  val_ptr_i = (long long *)vi;
  val_ptr_j = (long long *)vj;
  val_i = *val_ptr_i;
  val_j = *val_ptr_j;

  /* Output in descending order */
  if ( val_i < val_j )  {
    return (1);
  }
  else if ( val_i > val_j ) {
    return (-1);
  }
  else {
    return(0);
  }
}
