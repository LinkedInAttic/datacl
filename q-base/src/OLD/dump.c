#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "meta_data.h"
#include "aux_meta.h"
#include "lock_stuff.h"

extern char *g_docroot;
// START FUNC DECL
int
dump(
    char *tblfile,
    char *fldfile
	 )
// STOP FUNC DECL
{
  int status = 0;
  FILE *fp = NULL;
  TBL_REC_TYPE *x = NULL; FLD_REC_TYPE *y = NULL;
  char *X = NULL; size_t nX = 0;
  char str_fldtype[32];
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd = false;

  if ( ( tblfile == NULL ) || ( *tblfile == '\0' ) ) {  go_BYE(-1); }
  if ( ( fldfile == NULL ) || ( *fldfile == '\0' ) ) {  go_BYE(-1); }
  if ( strcmp(tblfile, fldfile) == 0 ) { go_BYE(-1); }
  fp = fopen(tblfile, "w");
  return_if_fopen_failed(fp, tblfile, "w");
  get_rd_lock();
#include "meta_map.h"
  status = mmap_meta_data(g_docroot, 
			  &tbl_X, &tbl_nX, &tbls, &n_tbl, 
			  &ht_tbl_X, &ht_tbl_nX, &ht_tbl, &n_ht_tbl, 
			  &fld_X, &fld_nX, &flds, &n_fld, 
			  &ht_fld_X, &ht_fld_nX, &ht_fld, &n_ht_fld);
  cBYE(status);
  for ( int i = 0; i < n_tbl; i++ ) { 
    x = &(tbls[i]);
    if ( ( x->name == NULL ) || ( x->name[0] == '\0' ) ) {
      continue;
    }
    fprintf(fp, "%d,%lld,%lld,", i, x->nR, x->magic_val);
    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", x->name);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */
    fprintf(fp, "\""); /* open quote */
    pr_disp_name(fp, x->dispname);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, "\n");  /* record delimiter */
  }
  fclose_if_non_null(fp);
  /*-------------------------------------------------*/
  fp = fopen(fldfile, "w");
  return_if_fopen_failed(fp, fldfile, "w");
  for ( int i = 0; i < n_fld; i++ ) { 
    y = &(flds[i]);
    if ( ( y->filename == NULL ) || ( y->filename[0] == '\0' ) ) {
      continue;
    }
    fprintf(fp, "%d,%d,%d,%d,", i, y->tbl_id, y->is_external, y->sort_type);
    if ( y->parent_id < 0 ) { 
      fprintf(fp, "\"\",");
    }
    else {
      fprintf(fp, "%d,", y->parent_id);
    }
    if ( y->nn_fld_id < 0 ) { 
      fprintf(fp, "\"\",");
    }
    else {
      fprintf(fp, "%d,", y->nn_fld_id);
    }
    zero_string(str_fldtype, 32);
    mk_str_fldtype(y->fldtype, str_fldtype);
    fprintf(fp, "\"%s\",", str_fldtype); /* fldtype */
    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", y->name);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */

    fprintf(fp, "\""); /* open quote */
    fprintf(fp, "%s", y->filename);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, ",");  /* field separator */

    if ( ( y->filename != NULL ) && ( y->filename[0] != '\0' ) ) {
      mcr_cd;
      status = rs_mmap(y->filename, &X, &nX, 0); 
      if ( ( status < 0 ) ||  ( nX == 0 ) ) { 
	status = 0;
        fprintf(fp, ","); /* indicating unknown size */
      }
      else {
        fprintf(fp, "%lld,", (long long)nX);
      }
      mcr_uncd;
      rs_munmap(X, nX);
    }
    else {
      fprintf(fp, ","); /* indicating unknown size */
    }

    fprintf(fp, "\""); /* open quote */
    pr_disp_name(fp, y->dispname);
    fprintf(fp, "\""); /* close quote */
    fprintf(fp, "\n");  /* record delimiter */
  }
  fclose_if_non_null(fp);
BYE:
  release_rd_lock();
  unmap_meta_data(tbl_X, tbl_nX, ht_tbl_X, ht_tbl_nX, 
		  fld_X, fld_nX, ht_fld_X, ht_fld_nX);
  fclose_if_non_null(fp);
  return(status);
}
