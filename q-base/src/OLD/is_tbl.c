#include "qtypes.h"
#include "lock_stuff.h"
#include "meta_data.h"
#include "hash_string.h"
#include "aux_meta.h"
#include "is_tbl.h"
#include "lock_stuff.h"

extern char *g_docroot;
// START FUNC DECL
int
core_is_tbl(
    const char *tbl,
    TBL_REC_TYPE *tbls,
    const int n_tbl,
    const META_KEY_VAL_TYPE *ht_tbl,
    const int n_ht_tbl,
    int *ptr_tbl_id, 
    TBL_REC_TYPE *ptr_tbl_rec
    )
// STOP FUNC DECL
{
  int status = 0;
  int idx, hashval, startloc;

  *ptr_tbl_id = INT_MIN;
  zero_tbl_rec(ptr_tbl_rec);
  status = hash_string(tbl, &hashval); cBYE(status);
  idx = hashval % n_ht_tbl;
  startloc = ht_tbl[idx].val;
  if ( startloc < 0 ) { startloc = 0; }
    
  for ( int i = startloc; i < n_tbl; i++ ) { 
    if ( strcmp(tbls[i].name, tbl) == 0 ) {
      *ptr_tbl_id = i;
      *ptr_tbl_rec = tbls[i];
      break;
    }
  }
  if ( *ptr_tbl_id == -1 ) { /* not found in above loop */
    for ( int i = 0; i < startloc; i++ ) { 
      if ( strcmp(tbls[i].name, tbl) == 0 ) {
	*ptr_tbl_id = i;
        *ptr_tbl_rec = tbls[i];
	break;
      }
    }
  }
BYE:
  return(status);
}

// START FUNC DECL
int
is_tbl(
       const char *tbl,
       int *ptr_tbl_id, /* negative means tbl does not exist */
       TBL_REC_TYPE *ptr_tbl_rec
       )
// STOP FUNC DECL
{
  int status = 0;
#include "meta_map.h"

  *ptr_tbl_id = -1;
  status  = get_rd_lock(); cBYE(status);
  status = mmap_meta_data(g_docroot, 
	&tbl_X, &tbl_nX, &tbls, &n_tbl, 
	&ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
	&fld_X, &fld_nX, &flds, &n_fld, 
	&ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, 
      ptr_tbl_id, ptr_tbl_rec);
  cBYE(status);

 BYE:
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
	fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  release_rd_lock();
  return(status);
}
