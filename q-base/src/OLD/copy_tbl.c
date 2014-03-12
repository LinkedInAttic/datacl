#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "tbl_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_tbl.h"
#include "copy_fld.h"
#include "del_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int
copy_tbl(
	 char *in_tbl,
	 char *out_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  int in_tbl_id, out_tbl_id;
  long long nR;
  int num_flds = 0; int *fld_ids = NULL;
  char str_nR[32];

  //------------------------------------------------
  zero_string(str_nR, 32);
  if ( ( in_tbl == NULL ) || ( *in_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( out_tbl == NULL ) || ( *out_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(in_tbl, out_tbl) == 0 ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(in_tbl, &in_tbl_id); cBYE(status);
  chk_range(in_tbl_id, 0, g_n_tbl);
  status = del_tbl(out_tbl, -1); cBYE(status);
  nR = g_tbl[in_tbl_id].nR;
  sprintf(str_nR, "%lld", nR);
  //------------------------------------------------
  // Make list of primary fields in in_tbl
  status = flds_in_tbl(in_tbl_id, false, &num_flds, &fld_ids);
  cBYE(status);
  //------------------------------------------------
  status = add_tbl(out_tbl, str_nR, &out_tbl_id);
  cBYE(status);
  for ( int i = 0; i < num_flds; i++ ) { 
    int fld_id; char *fldname;
    fld_id = fld_ids[i];
    fldname = g_fld[fld_id].name;
    if ( strcmp(g_fld[fld_id].fldtype, "char string") == 0 ) {
      fprintf(stderr, "Skipping field %s \n", fldname);
      continue;
    }
    status = copy_fld(in_tbl, fldname, "", out_tbl);
    cBYE(status);
  }

 BYE:
  free_if_non_null(fld_ids);
  return(status);
}
