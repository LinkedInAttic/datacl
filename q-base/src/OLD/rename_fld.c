#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "qtypes.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "aux_fld_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
rename_fld(
	char *tbl,
	char *old_fld,
	char *new_fld
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id, old_fld_id, new_fld_id, nn_fld_id, sz_fld_id;
  char new_aux_fld[MAX_LEN_FLD_NAME+1];
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( new_fld == NULL ) || ( *new_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( old_fld == NULL ) || ( *old_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(new_fld, old_fld) == 0 ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, old_fld, &old_fld_id); cBYE(status);
  chk_range(old_fld_id, 0, g_n_fld);
  status = is_fld(NULL, tbl_id, new_fld, &new_fld_id); cBYE(status);
  /* cannot rename an auxiliary field */
  if ( g_fld[old_fld_id].parent_id >= 0 ) { go_BYE(-1); }
  if ( new_fld_id >= 0 ) { /* delete new field if it exists */
    status = del_fld(NULL, tbl_id, NULL, new_fld_id); cBYE(status);
  }
  chk_fld_name(new_fld, 0);
  strcpy(g_fld[old_fld_id].name, new_fld);
  /* Now deal with nn field if it exists */
  nn_fld_id = g_fld[old_fld_id].nn_fld_id;
  if ( nn_fld_id >= 0 ) { 
    if ( ( strlen(new_fld) + strlen("_nn_") ) > MAX_LEN_FLD_NAME ) {
      go_BYE(-1);
    }
    strcpy(new_aux_fld, "_nn_");
    strcat(new_aux_fld, new_fld);
    strcpy(g_fld[nn_fld_id].name, new_aux_fld);
  }
  /* Now deal with sz field if it exists */
  sz_fld_id = g_fld[old_fld_id].sz_fld_id;
  if ( sz_fld_id >= 0 ) { 
    if ( ( strlen(new_fld) + strlen("_sz_") ) > MAX_LEN_FLD_NAME ) {
      go_BYE(-1);
    }
    strcpy(new_aux_fld, "_sz_");
    strcat(new_aux_fld, new_fld);
    strcpy(g_fld[sz_fld_id].name, new_aux_fld);
  }
  //--------------------------------------------------------
 BYE:
  return(status);
}
