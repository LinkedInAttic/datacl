/* Move a column (both data and meta data) from one table to the other. 
 * Note that it will no longer exist in source table */

#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "mv_fld.h"
#include "meta_globals.h"

// last review 9/12/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
mv_fld(
	 char *src_tbl,
	 char *src_fld,
	 char *dst_tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE src_tbl_rec, dst_tbl_rec;
  FLD_REC_TYPE src_fld_rec, nn_src_fld_rec;
  FLD_REC_TYPE dst_fld_rec, nn_dst_fld_rec;
  long long nR1 = LLONG_MIN, nR2  = LLONG_MIN;
  int src_tbl_id = INT_MIN, dst_tbl_id = INT_MIN; 
  int src_fld_id = INT_MIN, dst_fld_id = INT_MIN;
  int nn_src_fld_id = INT_MIN, nn_dst_fld_id = INT_MIN;

  //----------------------------------------------------------------
  if ( ( src_tbl == NULL ) || ( *src_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( src_fld == NULL ) || ( *src_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( dst_tbl == NULL ) || ( *dst_tbl == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(src_tbl, dst_tbl) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(src_tbl, &src_tbl_id, &src_tbl_rec); cBYE(status);
  chk_range(src_tbl_id, 0, g_n_tbl);
  nR1 = src_tbl_rec.nR;

  status = is_tbl(dst_tbl, &dst_tbl_id, &dst_tbl_rec); cBYE(status);
  chk_range(dst_tbl_id, 0, g_n_tbl);
  nR2 = dst_tbl_rec.nR;
  if ( nR1 != nR2 ) { go_BYE(-1); }

  status = is_fld(NULL, src_tbl_id, src_fld, &src_fld_id, &src_fld_rec, 
		  &nn_src_fld_id, &nn_src_fld_rec); cBYE(status);
  chk_range(src_fld_id, 0, g_n_fld);
  switch ( src_fld_rec.fldtype ) {
    case I1 : case I2 : case I4 : case I8 : case F4 : case F8 :  break;
    default : go_BYE(-1); break;
  }

  // Add field from to meta data store 
  zero_fld_rec(&dst_fld_rec); dst_fld_rec.fldtype = src_fld_rec.fldtype;
  status = add_fld(dst_tbl_id, src_fld, src_fld_rec.ddir_id, 
      src_fld_rec.fileno, &dst_fld_id, &dst_fld_rec);
  cBYE(status);
  /* some meta data stays the same */
  status = copy_meta(dst_fld_id, src_fld_id, 1); cBYE(status);
  if ( nn_src_fld_id >= 0 ) { 
    zero_fld_rec(&nn_dst_fld_rec); nn_dst_fld_rec.fldtype = I1;
    status = add_aux_fld(NULL, dst_tbl_id ,NULL, dst_fld_id, 
	nn_src_fld_rec.ddir_id, nn_src_fld_rec.fileno, nn, 
	&nn_dst_fld_id, &nn_dst_fld_rec);
    cBYE(status);
  }
  /* Clear out src_fld meta data */
  zero_fld_rec(&(g_flds[src_fld_id]));
  if ( nn_src_fld_id >= 0 ) {
    zero_fld_rec(&(g_flds[nn_src_fld_id]));
  }
  status = del_from_fld_info(src_fld_id); cBYE(status);
 BYE:
  return(status);
}
