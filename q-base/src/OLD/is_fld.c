#include "qtypes.h"
#include "lock_stuff.h"
#include "meta_data.h"
#include "hash_string.h"
#include "is_tbl.h"
#include "aux_meta.h"
#include "is_fld.h"

extern char *g_docroot;
// START FUNC DECL
int
  core_is_fld(
      int tbl_id,
      const char *fld,
      FLD_REC_TYPE *flds,
      int n_fld,
      META_KEY_VAL_TYPE *ht_fld,
      int n_ht_fld,
     int *ptr_fld_id,
     FLD_REC_TYPE *ptr_fld_rec,
     int *ptr_nn_fld_id,
     FLD_REC_TYPE *ptr_nn_fld_rec
      )
// STOP FUNC DECL
{
  int status = 0;
  int idx, hashval, startloc;

  *ptr_fld_id = -1;
  *ptr_nn_fld_id = -1;
  zero_fld_rec(ptr_fld_rec);
  zero_fld_rec(ptr_nn_fld_rec);
  status = hash_string(fld, &hashval); cBYE(status);
  idx = hashval % n_ht_fld;
  startloc = ht_fld[idx].val;
  if ( startloc < 0 ) { startloc = 0; }
    
  for ( int i = startloc; i < n_fld; i++ ) { 
    if ( flds[i].tbl_id != tbl_id ) { continue; }
    if ( flds[i].is_dirty == TRUE ) { continue; }
    if ( strcmp(flds[i].name, fld) == 0 ) {
      *ptr_fld_id = i;
      *ptr_fld_rec = flds[i];
      break;
    }
  }
  if ( *ptr_fld_id == -1 ) { /* not found in above loop */
    for ( int i = 0; i < startloc; i++ ) { 
      if ( flds[i].tbl_id != tbl_id ) { continue; }
      if ( flds[i].is_dirty == TRUE ) { continue; }
      if ( strcmp(flds[i].name, fld) == 0 ) {
	*ptr_fld_id = i;
        *ptr_fld_rec = flds[i];
	break;
      }
    }
  }
BYE:
  return(status);
}
// START FUNC DECL
int
is_fld(
       const char *tbl,
	int tbl_id,
       const char *fld,
       int *ptr_fld_id, /* negative means fld does not exist */
       FLD_REC_TYPE *ptr_fld_rec,
       int *ptr_nn_fld_id, /* negative means fld does not exist */
       FLD_REC_TYPE *ptr_nn_fld_rec

       )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec; 

  *ptr_fld_id = -1;
  zero_fld_rec(ptr_fld_rec);

  status  = get_rd_lock(); cBYE(status);

#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
	&tbl_X, &tbl_nX, &tbls, &n_tbl, 
	&ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
	&fld_X, &fld_nX, &flds, &n_fld, 
	&ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
    cBYE(status);
  if ( tbl_id < 0 ) { 
    status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, 
      &tbl_id, &tbl_rec);
    cBYE(status);
  }
  if ( tbl_id < 0 ) { goto BYE; }
  //---------------------------------------------------------
  status = core_is_fld(tbl_id, fld, flds, n_fld, ht_fld, n_ht_fld,
      ptr_fld_id, ptr_fld_rec, ptr_nn_fld_id, ptr_nn_fld_rec); 
  cBYE(status);
  
 BYE:
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
	fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  release_rd_lock();
  return(status);
}
