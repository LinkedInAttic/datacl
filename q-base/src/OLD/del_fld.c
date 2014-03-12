#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "meta_data.h"
#include "lock_stuff.h"

extern char *g_docroot;
// START FUNC DECL
int  
core_del_fld(
	     const char *tbl,
	     int tbl_id,
	     const char *fld,
	     int fld_id,
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
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char cwd[MAX_LEN_DIR_NAME];
  int nn_fld_id = -1;
  // Make sure that table exists 
  if ( tbl_id < 0 ) {
    status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, 
			 &tbl_id, &tbl_rec); cBYE(status);
  }
  if ( tbl_id < 0 ) { /* Nothing to do */
    goto BYE;
  }
  //------------------------------------------------------
  if ( fld_id < 0 ) { 
    status = core_is_fld(tbl_id, fld, flds, n_fld, ht_fld, n_ht_fld,
			 &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); 
    cBYE(status);
  }
  if ( fld_id < 0 ) {  /* Nothing to do */
    return(status);
  }
  //--------------------------------------------
  // You should not be deleting auxiliary fields directly
  if ( flds[fld_id].auxtype != undef ) { /* auxiliary field */
    fprintf(stderr, "Field [%s] in Table [%s] is aux field.\n", fld, tbl);
    fprintf(stderr, "Delete primary field to delete this field \n");
    go_BYE(-1);
  }
  //------------------------------------------------
  /* Unlink storage if internal field  */
  if ( flds[fld_id].is_external == false ) { 
    zero_string(cwd, MAX_LEN_DIR_NAME);
    if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
    chdir(g_docroot);
    unlink(flds[fld_id].filename);
    chdir(cwd);
  }
  zero_fld_rec(&(flds[fld_id])); /* Delete entry for this field  */
  if ( nn_fld_id < 0 ) { 
    nn_fld_id = nn_fld_rec.nn_fld_id;
  }
  // Delete nn field if necessary
  if ( nn_fld_id >= 0 ) {
    if ( flds[fld_id].is_external == false ) { 
      zero_string(cwd, MAX_LEN_DIR_NAME);
      if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
      chdir(g_docroot);
      unlink(flds[nn_fld_id].filename);
      chdir(cwd);
    }
    zero_fld_rec(&(flds[nn_fld_id])); /* Delete entry for this field  */
  }
 BYE:
  return(status);
}
// START FUNC DECL
int
del_fld(
	char *tbl,
	int tbl_id,
	char *fld,
	int fld_id
	)
// STOP FUNC DECL
{
  int status = 0;

  //- Basic parameter checks
  if ( tbl_id < 0 ) { 
    if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  }
  if ( fld_id < 0 ) { 
    if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  }

  get_wr_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  status = core_del_fld( tbl, tbl_id, fld, fld_id, 
      tbls, n_tbl, ht_tbl, n_ht_tbl, flds, n_fld, ht_fld, n_ht_fld);
  cBYE(status);
 BYE:
  return(status);
}
