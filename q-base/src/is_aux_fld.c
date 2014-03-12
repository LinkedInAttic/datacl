#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"

// START FUNC DECL
int
is_aux_fld(
	char *tbl,
	char *fld,
	char *str_auxtype,
	int *ptr_aux_fld_id,
	char *str_result
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id; TBL_REC_TYPE tbl_rec;
  int fld_id; FLD_REC_TYPE fld_rec;
  int aux_fld_id = -1; 
  int nn_fld_id = -1; FLD_REC_TYPE nn_fld_rec;
  //------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
  cBYE(status);
  chk_range(fld_id, 0, g_n_fld);
  AUX_TYPE auxtype;
  status = unstr_auxtype(str_auxtype, &auxtype); cBYE(status);
  switch ( auxtype ) { 
    case  nn : 
      aux_fld_id = g_flds[fld_id].nn_fld_id; 
      break;
    case len : 
      status = get_fld_info(fk_fld_len, fld_id, &aux_fld_id); cBYE(status);
      break;
    case off : 
      status = get_fld_info(fk_fld_off, fld_id, &aux_fld_id); cBYE(status);
      break;
    default : go_BYE(-1); break;
  }
  if ( aux_fld_id >= 0 ) {
    sprintf(str_result, "1:%d", aux_fld_id);
  }
  else {
    strcpy(str_result, "0:-1");
  }
  *ptr_aux_fld_id = aux_fld_id;
  //------------------------------------------------
 BYE:
  return status ;
}
