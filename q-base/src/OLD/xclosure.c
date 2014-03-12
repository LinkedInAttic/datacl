/* Do one step of a transitive closure */
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
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "fld_meta.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "crossprod1.h"
#include "get_nR.h"

//---------------------------------------------------------------
int 
crossprod1(
      char *docroot,
      sqlite3 *in_db,
      char *t1,
      char *f1,
      char *f2
      )
{
  int status = 0;
  sqlite3 *db = NULL;

  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;

  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;

  int t1_id, t2_id, t3_id, itemp; bool b_is_tbl;
  FLD_META_TYPE f1_meta, f2_meta;
  FLD_META_TYPE nn_f1_meta, nn_f2_meta;
  int i1val, i2val; int *iptr1 = NULL, *iptr2 = NULL;
  long long nR1, nR2, nR3 = 0;
  char str_meta_data[1024];
  char *t3f1_opfile = NULL, *t3f2_opfile = NULL;
  FILE *ofp1 = NULL, *ofp2 = NULL;
  char str_nR3[32];
  //----------------------------------------------------------------
  zero_string(str_meta_data, 1024);
  zero_string(str_nR3, 32);
  zero_fld_meta(&f1_meta);
  zero_fld_meta(&f2_meta);
  zero_fld_meta(&nn_f1_meta);
  zero_fld_meta(&nn_f2_meta);
  //----------------------------------------------------------------
  status = open_temp_file(&ofp1, &t3f1_opfile);
  cBYE(status);
  status = open_temp_file(&ofp2, &t3f2_opfile);
  cBYE(status);
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  //----------------------------------------------------------------
  status = external_get_nR(docroot, db, t1, &nR1, &t1_id);
  cBYE(status);
  if ( nR1 <= 0 ) { go_BYE(-1); }
  status = fld_meta(docroot, db, t1, f1, -1, &f1_meta);
  cBYE(status);
  status = rs_mmap(f1_meta.filename, &f1_X, &f1_nX, 0);
  cBYE(status);

  // Get nn field for t1/f1 if it exists
  status = get_aux_field_if_it_exists(docroot, db, t1, f1_meta.id, 
      "nn", &nn_f1_meta, &nn_f1_X, &nn_f1_nX);
  //----------------------------------------------------------------
  status = external_get_nR(docroot, db, t2, &nR2, &t2_id);
  cBYE(status);
  if ( nR2 <= 0 ) { go_BYE(-1); }
  status = fld_meta(docroot, db, t2, f2, -1, &f2_meta);
  cBYE(status);
  status = rs_mmap(f2_meta.filename, &f2_X, &f2_nX, 0);
  cBYE(status);

  // Get nn field for t2/f2 if it exists
  status = get_aux_field_if_it_exists(docroot, db, t2, f2_meta.id, 
      "nn", &nn_f2_meta, &nn_f2_X, &nn_f2_nX);
  //----------------------------------------------------------------
  iptr1 = (int *)f1_X;
  iptr2 = (int *)f2_X;
  for ( long long i1 = 0; i1 < nR1; i1++ ) { 
    if ( ( nn_f1_X == NULL ) || ( nn_f1_X[i1] == 1 ) )  {
      i1val = iptr1[i1];
      for ( long long i2 = 0; i2 < nR2; i2++ ) { 
        if ( ( nn_f2_X == NULL ) || ( nn_f2_X[i2] == 1 ) )  {
          i2val = iptr2[i2];
	  fwrite(&i1val, sizeof(int), 1, ofp1);
	  fwrite(&i2val, sizeof(int), 1, ofp2);
	  nR3++;
	}
      }
    }
  }
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);

  //----------------------------------------------------------------
  // Add output fields to t3 meta data 
  status = is_tbl(docroot, db, t3, &b_is_tbl, &t3_id, &itemp);
  cBYE(status);
  if ( b_is_tbl ) {
    status = del_tbl(docroot, db, t3);
    cBYE(status);
  }
  if ( nR3 == 0 ) {
    fprintf(stderr, "No data to create t3 \n");
    goto BYE;
  }
  sprintf(str_nR3, "%lld", nR3);
  status = add_tbl(docroot, db, t3, str_nR3, &itemp);
  cBYE(status);

  sprintf(str_meta_data, "fldtype=int:n_sizeof=%d:filename=%s",
      sizeof(int), t3f1_opfile);
  status = add_fld(docroot, db, t3, f1, str_meta_data);
  cBYE(status);
  zero_string(str_meta_data, 1024);
  sprintf(str_meta_data, "fldtype=int:n_sizeof=%d:filename=%s",
      sizeof(int), t3f2_opfile);
  status = add_fld(docroot, db, t3, f2, str_meta_data);
  cBYE(status);
 BYE:
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  if ( in_db == NULL ) { sqlite3_close(db); }
  rs_munmap(f1_X, f1_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  free_if_non_null(t3f1_opfile);
  free_if_non_null(t3f2_opfile);
  return(status);
}
