#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sort_1_int.h"

int sort_1_a_int_compare(
    const void *vi,
    const void *vj
    )
{ 
  int val_i, val_j;
  int *val_ptr_i, *val_ptr_j;

  val_ptr_i = (int *)vi;
  val_ptr_j = (int *)vj;
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
int sort_1_d_int_compare(
    const void *vi,
    const void *vj
    )
{ 
  int val_i, val_j;
  int *val_ptr_i, *val_ptr_j;

  val_ptr_i = (int *)vi;
  val_ptr_j = (int *)vj;
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
