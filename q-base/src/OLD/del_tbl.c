#include "qtypes.h"
#include "dbauxil.h"
#include "del_fld.h"
#include "is_tbl.h"
#include "tbl_meta.h"
#include "aux_meta.h"
#include "meta_data.h"
#include "lock_stuff.h"
extern char *g_docroot;
// START FUNC DECL
int
  core_del_tbl(
      int tbl_id, 
	     TBL_REC_TYPE *tbls,
	     int n_tbl,
	     META_KEY_VAL_TYPE *ht_tbl,
	     int n_ht_tbl,
	     FLD_REC_TYPE *flds,
	     int n_fld,
	     META_KEY_VAL_TYPE *ht_fld,
	     int n_ht_fld
      )
// STOP FUNC DECL
{
  int status = 0;
  chk_range(tbl_id, 0, n_tbl);
  // Delete all the fields in this table 
  for ( int i = 0; i < n_fld; i++ ) { 
    if ( flds[i].tbl_id == tbl_id ) { 
      status = core_del_fld( NULL, tbl_id, NULL, i, 
      tbls, n_tbl, ht_tbl, n_ht_tbl, flds, n_fld, ht_fld, n_ht_fld);
    }
  }
  //------------------------------------------------
  // Delete table itself
  zero_tbl_rec(&(tbls[tbl_id]));
BYE:
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int
del_tbl(
    const char *tbl,
	int tbl_id
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;

  // Basic arg checks 
  if ( tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  }

  get_wr_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  //------------------------------------------------
  if ( tbl_id < 0 ) {
    status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, 
	&tbl_id, &tbl_rec); cBYE(status);
  }
  if ( tbl_id < 0 ) { /* Nothing to do */
    goto BYE;
  }
  //------------------------------------------------------
  status = core_del_tbl(tbl_id, tbls, n_tbl, ht_tbl, n_ht_tbl, 
      flds, n_fld, ht_fld, n_ht_fld); cBYE(status);
BYE:
  release_wr_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}
