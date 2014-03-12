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
#include "bin_search.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "is_a_in_b.h"
#include "aux_join.h"
#include "get_nR.h"

/* Assume that f1 is sorted ascending and so is f2 */
/* If both are sorted, we can use srt_join instead. Hence, deprecated */

//---------------------------------------------------------------
// START FUNC DECL
int 
is_a_in_b(
	  char *docroot,
	  sqlite3 *in_db,
	  char *t1,
	  char *f1,
	  char *t2,
	  char *f2,
	  char *cfld
	  )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *nn_f1_X = NULL; size_t nn_f1_nX = 0;
  char *nn_f2_X = NULL; size_t nn_f2_nX = 0;
  FLD_META_TYPE f1_meta, f2_meta;
  FLD_META_TYPE nn_f1_meta, nn_f2_meta;
  int *if1 = NULL, *if2 = NULL;
  long long *llf1 = NULL, *llf2 = NULL;
  long long nR1, nR2;
  FILE *ofp = NULL; char *opfile = NULL;
  char str_meta_data[1024];
  int t1_id, t2_id;
  char ctrue = 1, cfalse = 0;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( cfld == NULL ) || ( *cfld == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(f1, cfld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  status = open_temp_file(&ofp, &opfile);
  //--------------------------------------------------------
  status = external_get_nR(docroot, db, t1, &nR1, &t1_id);
  cBYE(status);
  status = fld_meta(docroot, db, t1, f1, -1, &f1_meta);
  cBYE(status);
  status = rs_mmap(f1_meta.filename, &f1_X, &f1_nX, 0);
  cBYE(status);

  status = external_get_nR(docroot, db, t2, &nR2, &t2_id);
  cBYE(status);
  status = fld_meta(docroot, db, t2, f2, -1, &f2_meta);
  cBYE(status);
  status = rs_mmap(f2_meta.filename, &f2_X, &f2_nX, 0);
  cBYE(status);
  //--------------------------------------------------------
  if ( strcmp(f1_meta.fldtype, f2_meta.fldtype) != 0 ) {
    fprintf(stderr, "Fields being compared must be same type \n");
    go_BYE(-1); 
  } 
  //--------------------------------------------------------
  // START: Check that there are no null values
  status = get_aux_field_if_it_exists( docroot, db, t1, f1_meta.id, 
				       "nn", &nn_f1_meta, &nn_f1_X, &nn_f1_nX);
  cBYE(status);
  if ( nn_f1_X != NULL ) { 
    fprintf(stderr, "Field [%s] in Table [%s] has null values\n", f1, t1);
    fprintf(stderr, "NOT IMPLEMENTED\n");
    go_BYE(-1);
  }
  status = get_aux_field_if_it_exists( docroot, db, t2, f2_meta.id, 
				       "nn", &nn_f2_meta, &nn_f2_X, &nn_f2_nX);
  cBYE(status);
  if ( nn_f2_X != NULL ) { 
    fprintf(stderr, "Field [%s] in Table [%s] has null values\n", f2, t2);
    fprintf(stderr, "NOT IMPLEMENTED\n");
    go_BYE(-1);
  }
  // STOP: Check that there are no null values


  if1  = (int *)f1_X;
  if2  = (int *)f2_X;
  llf1 = (long long *)f1_X;
  llf2 = (long long *)f2_X;

  if ( strcmp(f1_meta.fldtype, "int") == 0 ) {
    if ( f1_meta.n_sizeof != sizeof(int) ) { go_BYE(-1); }
    long long i2 = 0;
    for ( long long i1 = 0; i1 < nR1; i1++ ) { 
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
        fwrite(&cfalse, sizeof(char), 1, ofp);
	continue;
      }
      if ( i2 >= nR2 ) {
        fwrite(&cfalse, sizeof(char), 1, ofp);
	continue;
      }
      for ( ; ; ) { 
	if ( if1[i1] == if2[i2] ) {
	  fwrite(&ctrue, sizeof(char), 1, ofp);
	  break;
	}
	else if ( if1[i1] > if2[i2] ) {
	  i2++;
	  if ( i2 == nR2 ) {
	    fwrite(&cfalse, sizeof(char), 1, ofp);
	    break;
	  }
	}
	else {
	  fwrite(&cfalse, sizeof(char), 1, ofp);
	  break;
	}
      }
    }
  }
  else if ( strcmp(f1_meta.fldtype, "long long") == 0 ) {
    if ( f1_meta.n_sizeof != sizeof(long long) ) { go_BYE(-1); }
    long long i2 = 0;
    for ( long long i1 = 0; i1 < nR1; i1++ ) { 
      if ( ( nn_f1_X != NULL ) && ( nn_f1_X[i1] == FALSE ) ) {
        fwrite(&cfalse, sizeof(char), 1, ofp);
	continue;
      }
      if ( i2 >= nR2 ) {
        fwrite(&cfalse, sizeof(char), 1, ofp);
	continue;
      }
      for ( ; ; ) { 
	if ( llf1[i1] == llf2[i2] ) {
	  fwrite(&ctrue, sizeof(char), 1, ofp);
	  break;
	}
	else if ( llf1[i1] > llf2[i2] ) {
	  i2++;
	  if ( i2 == nR2 ) {
	    fwrite(&cfalse, sizeof(char), 1, ofp);
	    break;
	  }
	}
	else {
	  fwrite(&cfalse, sizeof(char), 1, ofp);
	  break;
	}
      }
    }
  }
  else {
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
  }
  fclose_if_non_null(ofp);
  // Add output field to meta data 
  sprintf(str_meta_data, "n_sizeof=1:fldtype=bool:filename=%s", opfile);
  status = add_fld(docroot, db, t1, cfld, str_meta_data);
  cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(nn_f1_X, nn_f1_nX);
  rs_munmap(nn_f2_X, nn_f2_nX);
  if ( in_db == NULL ) { sqlite3_close(db); }
  free_if_non_null(opfile);
  fclose_if_non_null(ofp);
  return(status);
}
