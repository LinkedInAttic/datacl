#include "qtypes.h"
#include "meta_data.h"
#include "lock_stuff.h"
#include "dbauxil.h"

extern char *g_docroot;
// START FUNC DECL
int
list_tbls(
	 )
// STOP FUNC DECL
{
  int status = 0;
  #include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
	&tbl_X, &tbl_nX, &tbls, &n_tbl, 
	&ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
	&fld_X, &fld_nX, &flds, &n_fld, 
	&ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
    cBYE(status);

  get_rd_lock();
  for ( int i = 0; i < n_tbl; i++ ) { 
    if ( tbls[i].name[0] != '\0' ) { /* entry in use */
      fprintf(stdout, "%d,%lld,%s,\"", i, tbls[i].nR, tbls[i].name); /* open quote */
      pr_disp_name(stdout, tbls[i].dispname);
      fprintf(stdout, "\"\n"); /* close quote */
    }
  }
BYE:
  release_rd_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
	fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  return(status);
}
