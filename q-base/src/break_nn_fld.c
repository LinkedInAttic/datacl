/* Convert a field of type I1 into nn field of some other field */

#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_fld.h"
#include "aux_meta.h"
#include "break_nn_fld.h"
#include "meta_globals.h"
//
// last review 9/10/2013

//---------------------------------------------------------------
// START FUNC DECL
int 
break_nn_fld(
	 char *src_tbl,
	 char *src_fld,
	 char *dst_tbl,
	 char *dst_fld
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, nn_src_fld_rec;
  FLD_REC_TYPE dst_fld_rec; long long nR1 = LLONG_MIN, nR2  = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN;
  int nn_src_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_fld, dst_fld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  if ( src_tbl_id < 0 ) { go_BYE(-1); }
  nR1 = src_tbl_rec.nR;
  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  if (    src_fld_id < 0 ) { go_BYE(-1); }
  if ( nn_src_fld_id < 0 ) { go_BYE(-1); }

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  if ( dst_tbl_id < 0 ) { go_BYE(-1); }
  nR2 = dst_tbl_rec.nR;
  if ( nR1 != nR2 ) { go_BYE(-1); }

  zero_fld_rec(&dst_fld_rec); dst_fld_rec.fldtype = nn_src_fld_rec.fldtype; 
  status = add_fld(dst_tbl_id, dst_fld, nn_src_fld_rec.ddir_id, 
      nn_src_fld_rec.fileno, &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  /* Clear out nn_src_fld meta data */
  zero_fld_rec(&(g_flds[nn_src_fld_id]));
  g_flds[src_fld_id].nn_fld_id = -1;

 BYE:
  return status ;
}
