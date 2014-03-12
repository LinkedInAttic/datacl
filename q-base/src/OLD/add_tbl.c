#include "qtypes.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "meta_data.h"
#include "lock_stuff.h"
#include "auxil.h"
#include "is_tbl.h"

extern char *g_docroot;
// START FUNC DECL
int
get_empty_tbl(
	      TBL_REC_TYPE *tbls,
	      int n_tbl,
	      int *ptr_tbl_id
	      )
// STOP FUNC DECL
{
  int status = 0;

  // Pick a spot to start searching for an empty spot
  struct timeval Tps; struct timezone Tpf;
  gettimeofday(&Tps, &Tpf); 
  int startidx = Tps.tv_usec % n_tbl;

  *ptr_tbl_id = INT_MIN; /* no empty spot */
  for ( int i = startidx; i < n_tbl; i++ ) { 
    if ( tbls[i].name[0] == '\0' ) { 
      *ptr_tbl_id = i;
      break;
    }
  }
  if ( *ptr_tbl_id < 0 ) {
  for ( int i = 0; i < startidx; i++ ) { 
    if ( tbls[i].name[0] == '\0' ) { 
      *ptr_tbl_id = i;
      break;
    }
  }
  }
  if ( *ptr_tbl_id < 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n");
    go_BYE(-1);
  }
 BYE:
  return(status);
}

//---------------------------------------------------------------
// START FUNC DECL
int
add_tbl(
	char *tbl,
	char *str_nR,
	int *ptr_tbl_id,
	TBL_REC_TYPE *ptr_tbl_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  char *endptr = NULL;
  long long nR; int tbl_id;
  TBL_REC_TYPE tbl_rec;
  //---------------------- Check inputs
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_nR == NULL ) || ( *str_nR == '\0' ) ) {
    nR = 0;
  }
  else {
    nR = strtoll(str_nR, &endptr, 10);
    if ( nR < 0 ) { go_BYE(-1); } /* nR = 0 => empty table */
  }
  //------------------------------------------------------
  status = chk_tbl_name(tbl); cBYE(status);
  get_wr_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, 
      &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id >= 0 ) { /* Delete the table */
    status = core_del_tbl(tbl_id, tbls, n_tbl, ht_tbl, n_ht_tbl, 
	flds, n_fld, ht_fld, n_ht_fld); cBYE(status);
  }
  else {
    status = get_empty_tbl(tbls, n_tbl, &tbl_id); cBYE(status);
  }
  strcpy(tbls[tbl_id].name, tbl);
  tbls[tbl_id].nR = nR;
  tbls[tbl_id].magic_val = mk_magic_val();
  *ptr_tbl_id = tbl_id;
  *ptr_tbl_rec = tbls[tbl_id];
 BYE:
  release_wr_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}
