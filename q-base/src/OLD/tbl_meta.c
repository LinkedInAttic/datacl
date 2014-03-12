#include "qtypes.h"
#include "mmap.h"
#include "is_tbl.h"
#include "meta_data.h"
#include "lock_stuff.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "tbl_meta.h"

extern char *g_docroot; 
// START FUNC DECL
int
tbl_meta(
	 char *tbl
	 )
// STOP FUNC DECL
{
  int status = 0;
  int tbl_id = -1; TBL_REC_TYPE tbl_rec;
  char str_fldtype[32];

  //------------------------------------------------
  zero_string(str_fldtype, 32);
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  get_rd_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  status = core_is_tbl(tbl, tbls, n_tbl, ht_tbl, n_ht_tbl, &tbl_id, &tbl_rec);
  cBYE(status);
  if ( tbl_id < 0 ) { 
    fprintf(stderr, "Table [%s] not found\n", tbl); 
    go_BYE(-1);
  }
  //------------------------------------------------
  for ( int i = 0; i < n_fld; i++ ) { 
    if ( flds[i].tbl_id == tbl_id ) { 
      fprintf(stdout, "%d,%s,\"", i, flds[i].name);
      pr_disp_name(stdout, flds[i].dispname);
      status = mk_str_fldtype(flds[i].fldtype, str_fldtype);
      fprintf(stdout, "\",%s\n", str_fldtype);
    }
  }

  //------------------------------------------------
 BYE:
  release_rd_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}
