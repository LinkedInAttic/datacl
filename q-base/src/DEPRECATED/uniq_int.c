#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "uniq_int.h"

int uniq_int(
    char *in_X,
    long long n_in,
    long long *ptr_n_out,
    char *nn_X,
    char **ptr_opfile,
    char *str_meta_data
    )
{ 
  int status = 0;
  char *opfile = NULL;
  int *X = NULL;
  int n_out = 0;
  FILE *fp = NULL;
  bool is_good_val_seen = false;
  int last_good_val = 0; // initialization to suppress compiler warning

  status = open_temp_file(&fp, &opfile);
  cBYE(status);
  if ( in_X == NULL ) { go_BYE(-1); }
  X = (int *)in_X;
  for ( long long i = 0; i < n_in ; i++ ) { 
    if ( ( nn_X != NULL ) && ( nn_X[i] == FALSE ) ) {
      continue;
    }
    if ( is_good_val_seen == false ) {
      is_good_val_seen = true;
      fwrite(X+i, sizeof(int), 1, fp);
      last_good_val = X[i];
      n_out++;
    }
    else {
      if ( X[i] == last_good_val ) {
	/* nothing to do */
	/* 
        sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=%d",
        opfile, sizeof(long long));
	*/
      }
      else {
        fwrite(X+i, sizeof(int), 1, fp);
        last_good_val = X[i];
        n_out++;
      }
    }
  }
  fclose_if_non_null(fp);
  if ( n_out == 0 ) {
    unlink(opfile);
    free_if_non_null(opfile);
  }
  sprintf(str_meta_data, "filename=%s:fldtype=int:n_sizeof=%u",
      opfile, sizeof(int));
  
  *ptr_n_out = n_out;
  *ptr_opfile = opfile;
BYE:
  fclose_if_non_null(fp);
  return(status);
}
