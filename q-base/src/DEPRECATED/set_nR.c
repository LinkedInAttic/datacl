#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "tbl_meta.h"
#include "meta_globals.h"

// last review 9/22/2013

// START FUNC DECL
int
set_nR(
	char *tbl,
	char *str_nR
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id; TBL_REC_TYPE tbl_rec;
  long long nR; 
  int fld_ids[MAX_NUM_FLDS]; int num_flds = 0;
  //------------------------------------------------
  if ( ( str_nR == NULL ) || ( *str_nR == '\0' ) ) { go_BYE(-1); }
  status = stoI8(str_nR, &nR); cBYE(status);
  if ( nR <= 0 ) { go_BYE(-1);}
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec);
  cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(status); }
  status = tbl_meta(tbl, false, fld_ids, &num_flds);
  if ( num_flds > 0 ) { go_BYE(-1); }
  g_tbls[tbl_id].nR = nR;
 BYE:
  return(status);
}
