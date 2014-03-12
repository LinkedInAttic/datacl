#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "read_meta_data.h"
#include "aux_fld_meta.h"
#include "read_csv.h"
#include "orig_meta_globals.h"

bool g_write_to_temp_dir = false;
int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  FLD_TYPE *flds = NULL;
  int n_flds; long long num_rows;
  char *meta_data_file = NULL;
  char *data_file = NULL;
  char *str_load_fld = NULL;
  char **nn_fnames = NULL, **sz_fnames = NULL;

  if ( argc != 4 ) { go_BYE(-1); }
  meta_data_file = argv[1];
  data_file = argv[2];
  str_load_fld = argv[3];
  status = read_meta_data(meta_data_file, &flds, &n_flds);
  cBYE(status);

  fprintf(stdout, "n_flds = %d \n", n_flds);
  status = read_csv(flds, n_flds, data_file, str_load_fld, '"', ',',
      '\n', true, false, false, &nn_fnames, &sz_fnames, &num_rows);
  cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    pr_fld_meta(flds[i]);
    fprintf(stderr, "DBG ----   Field %d  ------------------\n", i);
    if ( sz_fnames[i] != NULL ) {
      fprintf(stdout, "Aux sz field exists. file = %s \n", sz_fnames[i]);
    }
    if ( nn_fnames[i] != NULL ) {
      fprintf(stdout, "Aux nn field exists. file = %s \n", nn_fnames[i]);
    }
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    free_if_non_null(nn_fnames[i]);
    free_if_non_null(sz_fnames[i]);
  }
  free_if_non_null(nn_fnames);
  free_if_non_null(sz_fnames);
BYE:
  free_if_non_null(flds);
  return(status);
}
