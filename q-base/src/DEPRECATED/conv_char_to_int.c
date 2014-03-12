#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "conv_char_to_int.h"

int
conv_char_to_int(
    char *in_X,
    char *nn_in_X,
    int nR,
    FILE *ofp,
    FILE *nn_ofp,
    bool *ptr_is_any_null
    )
{
  int status = 0;
  char *in_ptr = NULL;
  int lval;
  char nnval;

  *ptr_is_any_null = false;
  if ( in_X == NULL ) { go_BYE(-1); }
  if ( nR <= 0 ) { go_BYE(-1); }
  in_ptr  = (char *)in_X;
  for ( int i = 0; i < nR; i++ ) { 
    if ( ( nn_in_X != NULL ) && ( nn_in_X[i] == FALSE ) ) { 
      lval = 0;
      nnval = 0;
      *ptr_is_any_null = true;
    }
    else {
      lval = (int )(in_ptr[i]);
      nnval = 0;
    }
    fwrite(&lval, sizeof(int), 1, ofp);
    fwrite(&nnval, sizeof(char), 1, nn_ofp);
  }
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
BYE:
  return(status);
}
