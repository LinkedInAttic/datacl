#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "tbl_meta.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
chk_tbl(
    char *tbl
    )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id;
  int num_flds = 0; int *fld_ids = NULL;
  FLD_TYPE *x_fld_meta;
  long long nR;

  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  status = is_tbl(tbl, &tbl_id);
  cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = g_tbl[tbl_id].nR;
  if ( nR <= 0 ) { go_BYE(-1); }
  bool include_aux_flds = false;
  status = flds_in_tbl(tbl_id, include_aux_flds, &num_flds, &fld_ids); cBYE(status);
  if ( num_flds == 0 ) { go_BYE(-1); }
  if ( fld_ids == NULL ) { go_BYE(-1); }

  for ( int i = 0; i  < num_flds; i++ ) { 
    char *X = NULL; size_t nX = 0;
    int fld_id;
    fld_id = fld_ids[i];
    if ( fld_id <= 0 ) { go_BYE(-1); }
    x_fld_meta = &(g_fld[fld_id]);
    status = chk_fld_meta(*x_fld_meta, false, nR);
    cBYE(status);
    status = rs_mmap(x_fld_meta->filename, &X, &nX, 0);
    if ( status < 0 ) {
      fprintf(stderr, "Unable to find file %s \n", x_fld_meta->filename);
      go_BYE(-1);
    }
    if ( nX == 0 ) { 
      fprintf(stderr, "Empty file %s \n", x_fld_meta->filename);
      go_BYE(-1);
    }
    rs_munmap(X, nX);
  }

BYE:
  free_if_non_null(fld_ids);
  return(status);
}
