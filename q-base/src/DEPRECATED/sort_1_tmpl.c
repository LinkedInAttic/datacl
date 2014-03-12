#include <stdlib.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sort_1_TEXT2.h"

int sort_1_a_TEXT2_compare(
    const void *vi,
    const void *vj
    )
{ 
  TEXT1 val_i, val_j;
  TEXT1 *val_ptr_i, *val_ptr_j;

  val_ptr_i = (TEXT1 *)vi;
  val_ptr_j = (TEXT1 *)vj;
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
int sort_1_d_TEXT2_compare(
    const void *vi,
    const void *vj
    )
{ 
  TEXT1 val_i, val_j;
  TEXT1 *val_ptr_i, *val_ptr_j;

  val_ptr_i = (TEXT1 *)vi;
  val_ptr_j = (TEXT1 *)vj;
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
