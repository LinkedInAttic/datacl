#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "bin_search.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_a_in_b_2.h"
#include "aux_join.h"
#include "bin_search.h"
#include "get_nR.h"
#include "aux_fld_meta.h"

/* Assume that f1 is sorted ascending and so is f2 */
/* Assume that neither f1 nor f2 have null values */
/* src fld must be either int or long. Will later extend other fixed
 * length fields */

//---------------------------------------------------------------
// START FUNC DECL
int 
new_is_a_in_b_2(
	    char *docroot,
	    sqlite3 *in_db,
	    char *t1,
	    char *f1,
	    char *t2,
	    char *f2,
	    char *out_fld, /* boolean field created */
	    char *src_f2, /* fld_to_fetch_from_t2 */
	    char *dst_f1 /* fld_to_place_in_t1 */
	    )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *src_f2_X = NULL; size_t src_f2_nX = 0;
  char *nn_src_f2_X = NULL; size_t nn_src_f2_nX = 0;
  FLD_META_TYPE f1_meta, f2_meta, src_f2_meta, nn_src_f2_meta;
  long long nR1, nR2;
  char *opfile = NULL, *dst_opfile = NULL, *nn_dst_opfile = NULL;
  char str_meta_data[1024];
  int t1_id, t2_id;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( out_fld == NULL ) || ( *out_fld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, out_fld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  zero_string(str_meta_data, 1024);
  zero_fld_meta(&f1_meta);
  zero_fld_meta(&f2_meta);
  zero_fld_meta(&src_f2_meta);
  zero_fld_meta(&nn_src_f2_meta);
  //--------------------------------------------------------
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //--------------------------------------------------------
  // load f1 
  status = external_get_nR(docroot, db, t1, &nR1, &t1_id);
  cBYE(status);
  status = fld_meta(docroot, db, t1, f1, -1, &f1_meta);
  cBYE(status);
  status = rs_mmap(f1_meta.filename, &f1_X, &f1_nX, 0);
  cBYE(status);

  // load f2 
  status = external_get_nR(docroot, db, t2, &nR2, &t2_id);
  cBYE(status);
  status = fld_meta(docroot, db, t2, f2, -1, &f2_meta);
  cBYE(status);
  status = rs_mmap(f2_meta.filename, &f2_X, &f2_nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  // Check types of f1 and f2 are same 
  if ( strcmp(f1_meta.fldtype, f2_meta.fldtype) != 0 ) {
    fprintf(stderr, "Fields being compared must be same type \n");
    go_BYE(-1); 
  } 
  // load src_f2 is provided 
  if ( ( src_f2 != NULL ) && ( *src_f2 != '\0' ) ) { 
    status = fld_meta(docroot, db, t2, src_f2, -1, &src_f2_meta);
    cBYE(status);
    status = rs_mmap(src_f2_meta.filename, &src_f2_X, &src_f2_nX, 0);
    cBYE(status);
    status = get_aux_field_if_it_exists(
	docroot, db, t2, src_f2_meta.id, "nn", &nn_src_f2_meta, 
	&nn_src_f2_X, &nn_src_f2_nX);
    cBYE(status);
  }
  //--------------------------------------------------------
  par_is_in_b(f1_X, f1_meta, nR1, f2_X,  f2_meta, nR2, src_f2_X, 
      nn_src_f2_X, src_f2_meta, &opfile, &dst_opfile, &nn_dst_opfile);

  // Add output field to meta data 
  sprintf(str_meta_data, "n_sizeof=1:fldtype=bool:filename=%s", opfile);
  status = add_fld(docroot, db, t1, out_fld, str_meta_data);
  cBYE(status);
  if ( dst_opfile != NULL ) { 
    sprintf(str_meta_data, "n_sizeof=%u:fldtype=%s:filename=%s", 
	src_f2_meta.n_sizeof, src_f2_meta.fldtype, dst_opfile);
    status = add_fld(docroot, db, t1, dst_f1, str_meta_data);
    if ( nn_dst_opfile != NULL ) { 
      status = add_aux_fld(docroot, db, t1, dst_f1, nn_dst_opfile, "nn");
      cBYE(status);
    }
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(src_f2_X, src_f2_nX);
  rs_munmap(nn_src_f2_X, nn_src_f2_nX);
  if ( in_db == NULL ) { sqlite3_close(db); }
  free_if_non_null(opfile);
  free_if_non_null(dst_opfile);
  free_if_non_null(nn_dst_opfile);
  return(status);
}
