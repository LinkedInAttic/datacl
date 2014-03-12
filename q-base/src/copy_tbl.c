#include "qtypes.h"
#include "mmap.h"
#include "tbl_meta.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "fld_meta.h"
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
  TBL_REC_TYPE in_tbl_rec, out_tbl_rec;
  long long nR; char str_nR[32];
  int num_flds = 0; int fld_ids[MAX_NUM_FLDS_IN_TBL];

  //------------------------------------------------
  zero_string(str_nR, 32);
  if ( ( in_tbl == NULL ) || ( *in_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( out_tbl == NULL ) || ( *out_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(in_tbl, out_tbl) == 0 ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(in_tbl, &in_tbl_id, &in_tbl_rec); cBYE(status);
  chk_range(in_tbl_id, 0, g_n_tbl);
  status = del_tbl(out_tbl, -1); cBYE(status);
  nR = in_tbl_rec.nR;
  sprintf(str_nR, "%lld", nR);
  //------------------------------------------------
  // Make list of primary fields in in_tbl
  status = tbl_meta(in_tbl, false, fld_ids, &num_flds);
  //------------------------------------------------
  status = add_tbl(out_tbl, str_nR, &out_tbl_id, &out_tbl_rec); cBYE(status);
  for ( int i = 0; i < num_flds; i++ ) { 
    int fld_id; char *fldname = NULL;
    fld_id = fld_ids[i];
    FLD_REC_TYPE fld_rec = g_flds[fld_id];
    fldname = g_flds[fld_id].name;
    bool skip_fld = false;
    switch ( fld_rec.fldtype ) { 
      case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : break;
      default : skip_fld = true; break;
    }
    if ( skip_fld == true ) { continue; }
    status = copy_fld(in_tbl, fldname, NULL, out_tbl);
    cBYE(status);
  }

 BYE:
  return status ;
}
