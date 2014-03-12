#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "aux_meta.h"
#include "meta_globals.h"

// last review 9/10/2013
// START FUNC DECL
int
drop_aux_fld(
	   char *tbl,
	   char *fld,
	   char *str_auxtype,
	   int in_fld_id
	   )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = INT_MIN, fld_id = INT_MIN, nn_fld_id = INT_MIN;
  TBL_REC_TYPE tbl_rec; FLD_REC_TYPE fld_rec, nn_fld_rec;
  AUX_TYPE auxtype;
  //------------------------------------------------
  status = unstr_auxtype(str_auxtype, &auxtype); cBYE(status);
  if ( in_fld_id >= 0 ) { 
    fld_id = in_fld_id;
  }
  else {
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
    if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
    status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
    chk_range(tbl_id, 0, g_n_tbl);
    status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
    if ( fld_id < 0 ) { 
      fprintf(stderr, "Field %s in Table %s does not exist\n", fld, tbl); 
      WHEREAMI;
      return status ;
    }
  }
  chk_range(fld_id, 0, g_n_fld);
  status = del_aux_fld(fld_id, auxtype); cBYE(status);
 BYE:
  return status ;
}
