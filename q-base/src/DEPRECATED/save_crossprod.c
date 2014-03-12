#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "get_nR.h"
#include "f_to_s.h"
#include "mk_file.h"

#include "core_crossprod_I_I.h"
#include "core_crossprod_I_L.h"
#include "core_crossprod_L_I.h"
#include "core_crossprod_L_L.h"

static int get_data(
		    char *docroot,
		    sqlite3 *db,
		    char *tbl,
		    char *fld,
		    long long *ptr_nR,
		    FLD_META_TYPE *ptr_fld_meta,
		    char **ptr_X,
		    size_t *ptr_nX,
		    FLD_META_TYPE *ptr_nn_fld_meta,
		    char **ptr_nn_X,
		    size_t *ptr_nn_nX,
		    long long *ptr_nn_nR
		    )
{
  int status = 0;
  int tbl_id, fld_id;
  bool exists;
  char *X = NULL;    size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0;
  char *endptr = NULL;
  char buffer[32];

  zero_string(buffer, 32);

  status = is_tbl_fld(docroot, db, tbl, fld, &tbl_id, &fld_id, &exists);
  cBYE(status);
  if ( !exists ) { go_BYE(-1); }
  status = internal_get_nR(db, tbl_id, ptr_nR);
  status = fld_meta(docroot, db, tbl, fld, -1, ptr_fld_meta);
  cBYE(status);
  status = rs_mmap(ptr_fld_meta->filename, &X, &nX, 0);
  cBYE(status);
  status = get_aux_field_if_it_exists(docroot, db, tbl, fld_id, 
				      "nn", ptr_nn_fld_meta, &nn_X, &nn_nX);
  if ( nn_X == NULL ) { 
    *ptr_nn_nR = *ptr_nR;
  }
  else {
    status = f_to_s(docroot, db, tbl, ptr_nn_fld_meta->name, "sum", buffer);
    cBYE(status);
    *ptr_nn_nR = strtol(buffer, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( *ptr_nn_nR <= 0 ) { go_BYE(-1); }
  }
  *ptr_X  = X;       *ptr_nX = nX;
  *ptr_nn_X  = nn_X; *ptr_nn_nX = nn_nX;
 BYE:
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int 
crossprod(
	  char *docroot,
	  sqlite3 *in_db,
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *t3
	  )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;

  char *Y1 = NULL; size_t nY1 = 0;
  char *Y2 = NULL; size_t nY2 = 0;

  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;

  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;

  int t3_id, itemp; bool b_is_tbl;
  FLD_META_TYPE f1_meta, f2_meta;
  FLD_META_TYPE nn_f1_meta, nn_f2_meta;
  long long nR1, nR2, nn_nR1, nn_nR2, nR3;
  char str_meta_data[1024];
  char *t3f1_opfile = NULL, *t3f2_opfile = NULL;
  FILE *ofp = NULL;
  char buffer[32];
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  zero_string(buffer, 32);
  zero_fld_meta(&f1_meta);
  zero_fld_meta(&f2_meta);
  zero_fld_meta(&nn_f1_meta);
  zero_fld_meta(&nn_f2_meta);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //----------------------------------------------------------------
  status = get_data(docroot, db, t1, f1, &nR1, &f1_meta, &f1_X, 
      &f1_nX, &nn_f1_meta, &nn_f1_X, &nn_f1_nX, &nn_nR1);
  cBYE(status);
  status = get_data(docroot, db, t2, f2, &nR2, &f2_meta, &f2_X, 
      &f2_nX, &nn_f2_meta, &nn_f2_X, &nn_f2_nX, &nn_nR2);
  cBYE(status);
  nR3 = nn_nR1 * nn_nR2;
  if ( nR3 == 0 ) {
    fprintf(stderr, "No data to create t3 \n");
    goto BYE;
  }

  status = open_temp_file(&ofp, &t3f1_opfile); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(t3f1_opfile, nR3 * f1_meta.n_sizeof); cBYE(status);
  status = rs_mmap(t3f1_opfile, &Y1, &nY1, 1); cBYE(status);

  status = open_temp_file(&ofp, &t3f2_opfile); cBYE(status);
  fclose_if_non_null(ofp);
  status = mk_file(t3f2_opfile, nR3 * f2_meta.n_sizeof); cBYE(status);
  status = rs_mmap(t3f2_opfile, &Y2, &nY2, 1); cBYE(status);
  //----------------------------------------------------------------
  if ( ( strcmp(f1_meta.fldtype, "int") == 0 ) && 
     ( strcmp(f2_meta.fldtype, "int") == 0 ) ) {
    status = core_crossprod_I_I(f1_X, nn_f1_X, f2_X, nn_f2_X, nR1, nR2, nR3, Y1, Y2);
  }
  else if ( ( strcmp(f1_meta.fldtype, "int") == 0 ) && 
     ( strcmp(f2_meta.fldtype, "long long") == 0 ) ) {
    status = core_crossprod_I_L(f1_X, nn_f1_X, f2_X, nn_f2_X, nR1, nR2, nR3, Y1, Y2);
  }
  else if ( ( strcmp(f1_meta.fldtype, "long long") == 0 ) && 
     ( strcmp(f2_meta.fldtype, "int") == 0 ) ) {
    status = core_crossprod_L_I(f1_X, nn_f1_X, f2_X, nn_f2_X, nR1, nR2, nR3, Y1, Y2);
  }
  else if ( ( strcmp(f1_meta.fldtype, "long long") == 0 ) && 
     ( strcmp(f2_meta.fldtype, "long long") == 0 ) ) {
    status = core_crossprod_L_L(f1_X, nn_f1_X, f2_X, nn_f2_X, nR1, nR2, nR3, Y1, Y2);
  }
  else { go_BYE(-1); }
  cBYE(status);
  //----------------------------------------------------------------
  // Add output fields to t3 meta data 
  status = is_tbl(docroot, db, t3, &b_is_tbl, &t3_id);
  cBYE(status);
  if ( b_is_tbl ) {
    status = del_tbl(docroot, db, t3);
    cBYE(status);
  }
  sprintf(buffer, "%lld", nR3);
  status = add_tbl(docroot, db, t3, buffer, &itemp);
  cBYE(status);

  sprintf(str_meta_data, "fldtype=int:n_sizeof=%u:filename=%s",
	  f1_meta.n_sizeof, t3f1_opfile);
  status = add_fld(docroot, db, t3, f1, str_meta_data);
  cBYE(status);
  zero_string(str_meta_data, 1024);
  sprintf(str_meta_data, "fldtype=int:n_sizeof=%u:filename=%s",
	  f2_meta.n_sizeof, t3f2_opfile);
  status = add_fld(docroot, db, t3, f2, str_meta_data);
  cBYE(status);
 BYE:
  fclose_if_non_null(ofp);
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);

  rs_munmap(Y1, nY1);
  rs_munmap(Y2, nY2);

  free_if_non_null(t3f1_opfile);
  free_if_non_null(t3f2_opfile);
  return(status);
}
