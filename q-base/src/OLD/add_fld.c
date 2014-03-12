#include "qtypes.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "dbauxil.h"
#include "meta_data.h"
#include "aux_meta.h"
#include "auxil.h"
#include "del_fld.h"
#include "lock_stuff.h"

extern char *g_docroot;
// START FUNC DECL
int
get_empty_fld(
	      FLD_REC_TYPE *flds,
	      int n_fld,
	      int *ptr_fld_id
	      )
// STOP FUNC DECL
{
  int status = 0;
  //
  // Pick a spot to start searching for an empty spot
  struct timeval Tps; struct timezone Tpf;
  gettimeofday(&Tps, &Tpf); 
  int startidx = Tps.tv_usec % n_fld;

  *ptr_fld_id = INT_MIN; /* no empty spot */
  for ( int i = startidx; i < n_fld; i++ ) { 
    if ( flds[i].name[0] == '\0' ) { 
      *ptr_fld_id = i;
      break;
    }
  }
  if ( *ptr_fld_id < 0 ) {
  for ( int i = 0; i < startidx; i++ ) { 
    if ( flds[i].name[0] == '\0' ) { 
      *ptr_fld_id = i;
      break;
    }
  }
  }
  if ( *ptr_fld_id < 0 ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n");
    go_BYE(-1);
  }

 BYE:
  return(status);
}

// START FUNC DECL
int
add_fld(
	int tbl_id,
	long long tbl_magic_val,
	const char *fld,
	char *filename,
	FLD_TYPE fldtype,
	int *ptr_fld_id,
	FLD_REC_TYPE *ptr_fld_rec
	)
// STOP FUNC DECL
{
  int status = 0;
  FLD_REC_TYPE fld_rec;    int fld_id; 
  FLD_REC_TYPE nn_fld_rec; int nn_fld_id; 

  //------------------------------------------------
  *ptr_fld_id = -1;
  zero_fld_rec(ptr_fld_rec);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  status = chk_fld_name(fld, false);
  cBYE(status);
  //------------------------------------------------
  get_wr_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  status = chk_file_size(filename, tbls[tbl_id].nR, fldtype); cBYE(status);
  status = core_is_fld(tbl_id, fld, flds, n_fld, ht_fld, n_ht_fld, 
      &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
  if ( fld_id >= 0 ) { 
    status = core_del_fld(NULL, tbl_id, NULL, fld_id,
	tbls, n_tbl, ht_tbl, n_ht_tbl, 
	flds, n_fld, ht_fld, n_ht_fld);
    cBYE(status);
  }
  else {
    status = get_empty_fld(flds, n_fld, &fld_id); cBYE(status);
  }
  zero_fld_rec(&(flds[fld_id]));
  flds[fld_id].tbl_id      = tbl_id;
  flds[fld_id].is_external = false;
  flds[fld_id].sort_type   = unknown;
  flds[fld_id].parent_id   = INT_MIN;
  flds[fld_id].nn_fld_id   = INT_MIN;
  flds[fld_id].fldtype     = fldtype;
  flds[fld_id].auxtype     = undef;

  if ( strlen(filename) > MAX_LEN_FILE_NAME ) { go_BYE(-1); }
  strcpy(flds[fld_id].filename, filename);

  if ( strlen(fld) > MAX_LEN_FILE_NAME ) { go_BYE(-1); }
  strcpy(flds[fld_id].name, fld);

  flds[fld_id].magic_val    = mk_magic_val();

  *ptr_fld_id = fld_id;
  *ptr_fld_rec = flds[fld_id];
 BYE:
  release_wr_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}

// START FUNC DECL
int 
mark_fld_done(
    int tbl_id,
    long long tbl_magic_val,
    int fld_id,
    long long fld_magic_val
    )
// STOP FUNC DECL
{
  int status = 0;
  get_wr_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  if ( tbls[tbl_id].magic_val != tbl_magic_val ) { go_BYE(-1); }
  if ( flds[fld_id].magic_val != fld_magic_val ) { go_BYE(-1); }
  flds[fld_id].is_dirty = FALSE;
BYE:
  release_wr_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}


