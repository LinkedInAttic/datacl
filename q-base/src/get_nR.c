#include "qtypes.h"
#include "auxil.h" 
#include "dbauxil.h"
#include "is_tbl.h"
#include "meta_globals.h"

// START FUNC DECL
int
get_nR(
	char *tbl,
	long long *ptr_nR
	)
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id; TBL_REC_TYPE tbl_rec;
  //------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  *ptr_nR = g_tbls[tbl_id].nR;
 BYE:
  return status ;
}
//
// START FUNC DECL
int
int_get_nR(
	int tbl_id,
	long long *ptr_nR
	)
// STOP FUNC DECL
{
  int status = 0;
  //------------------------------------------------
  chk_range(tbl_id, 0, g_n_tbl);
  *ptr_nR = g_tbls[tbl_id].nR;
 BYE:
  return status ;
}
