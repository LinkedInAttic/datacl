#include "qtypes.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
rename_fld(
	const char *tbl,
	const char *old_fld,
	const char *new_fld
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id; 
  int old_fld_id, nn_old_fld_id; 
  int new_fld_id, nn_new_fld_id; 
  FLD_REC_TYPE old_fld_rec, nn_old_fld_rec;
  FLD_REC_TYPE new_fld_rec, nn_new_fld_rec;
  TBL_REC_TYPE tbl_rec;
  char new_aux_fld[MAX_LEN_FLD_NAME+1];
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( new_fld == NULL ) || ( *new_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( old_fld == NULL ) || ( *old_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(new_fld, old_fld) == 0 ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, old_fld, &old_fld_id, &old_fld_rec, 
      &nn_old_fld_id, &nn_old_fld_rec); cBYE(status);
  chk_range(old_fld_id, 0, g_n_fld);
  status = is_fld(NULL, tbl_id, new_fld, &new_fld_id, &new_fld_rec,
      &nn_new_fld_id, &nn_new_fld_rec); cBYE(status);
  /* If you rename an auxiliary field, you break the connection */
  int parent_id = g_flds[old_fld_id].parent_id;
  if ( parent_id >= 0 ) { /* old_fld is auxiliary fld */
    if ( g_flds[parent_id].nn_fld_id == old_fld_id ) {
      if ( ( g_flds[old_fld_id].fldtype != I1 ) && 
           ( g_flds[old_fld_id].fldtype != B ) ) { 
	go_BYE(-1);
      }
      g_flds[parent_id].nn_fld_id  = -1;
      g_flds[old_fld_id].parent_id = -1;
      g_flds[old_fld_id].auxtype = undef_auxtype;
    }
    else { 
      go_BYE(-1); 
    }
  }
  if ( new_fld_id >= 0 ) { /* delete new field if it exists */
    status = del_fld(NULL, tbl_id, NULL, new_fld_id); cBYE(status);
  }
  status = chk_fld_name(new_fld); cBYE(status);
  strcpy(g_flds[old_fld_id].name, new_fld);
  /* Now deal with nn field if it exists */
  if ( nn_old_fld_id >= 0 ) { 
    if ( ( strlen(new_fld) + strlen(".nn.") ) > MAX_LEN_FLD_NAME ) {
      go_BYE(-1);
    }
    strcpy(new_aux_fld, ".nn.");
    strcat(new_aux_fld, new_fld);
    strcpy(g_flds[nn_old_fld_id].name, new_aux_fld);
  }
 BYE:
  return(status);
}
