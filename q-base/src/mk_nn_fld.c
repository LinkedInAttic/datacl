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
#include "add_aux_fld.h"
#include "mk_nn_fld.h"
#include "drop_aux_fld.h"
#include "meta_globals.h"
//
// last review 9/10/2013

//---------------------------------------------------------------
// START FUNC DECL
int 
mk_nn_fld(
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
  FLD_REC_TYPE dst_fld_rec, nn_dst_fld_rec; long long nR1 = LLONG_MIN, nR2  = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN;
  int nn_src_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_fld == NULL ) || ( *dst_fld == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  if ( src_tbl_id < 0 ) { go_BYE(-1); }
  nR1 = src_tbl_rec.nR;

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  if ( dst_tbl_id < 0 ) { go_BYE(-1); }
  nR2 = dst_tbl_rec.nR;
  if ( nR1 != nR2 ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  if ( src_fld_id < 0 ) { go_BYE(-1); }
  switch ( src_fld_rec.fldtype ) { 
    case B : case I1 : break;
    default : go_BYE(-1); break;
  }
  if ( src_fld_rec.nn_fld_id >= 0 ) { go_BYE(-1); }
  // src fld should not currently be nn field of some other field
  if ( g_flds[src_fld_id].parent_id >= 0 ) { go_BYE(-1); }

  status = is_fld(NULL, dst_tbl_id, dst_fld, &dst_fld_id, &dst_fld_rec, 
		  &nn_dst_fld_id, &nn_dst_fld_rec); cBYE(status);
  if ( dst_fld_id < 0 ) { go_BYE(-1); }

  /* delete the nn field if it exists */
  if ( nn_dst_fld_id >= 0 ) { 
    status = drop_aux_fld(NULL, NULL, "nn", dst_fld_id); cBYE(status);
  }
  // Add src field as nn field of dst field 
  zero_fld_rec(&nn_dst_fld_rec); nn_dst_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_aux_fld(NULL, dst_tbl_id, NULL, dst_fld_id, 
	src_fld_rec.ddir_id, src_fld_rec.fileno, nn,
	&nn_dst_fld_id, &nn_dst_fld_rec);
  cBYE(status);
  /* Clear out src_fld meta data */
  zero_fld_rec(&(g_flds[src_fld_id]));
 BYE:
  return(status);
}
