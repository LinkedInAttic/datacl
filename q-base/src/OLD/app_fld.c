#include <stdio.h>
#include <unistd.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_aux_fld.h"
#include "add_fld.h"
#include "app_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "drop_null_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// Appends data of field f2 in table T2 to field f1 in Table t1
// START FUNC DECL
int 
app_fld(
	char *t1,
	char *f1,
	char *t2,
	char *f2
	)
// STOP FUNC DECL
{
  int status = 0;
  char *Y = NULL; size_t nY = 0;
  char *X2 = NULL; size_t nX2 = 0;
  FLD_TYPE *f1_fld_meta = NULL, *f2_fld_meta = NULL;
  FILE *ofp = NULL; char *opfile = NULL;
  char *nn_opfile = NULL, *sz_opfile = NULL;
  char nn_val;
  long long nR1, nR2; 
  int t1_id, f1_id, nn_f1_id, sz_f1_id;
  int t2_id, f2_id, nn_f2_id, sz_f2_id;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t1) == 0 ) { 
    if ( strcmp(f1, f2) != 0 ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;
  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbl[t2_id].nR;

  status = is_fld(NULL, t1_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_fld_meta = &(g_fld[f1_id]);

  status = is_fld(NULL, t2_id, f2, &f2_id); cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  f2_fld_meta = &(g_fld[f2_id]);
  //--------------------------------------------------------
  if ( strcmp(f1_fld_meta->fldtype, f2_fld_meta->fldtype) != 0 ) {
    fprintf(stderr, "Field1 [%s] in Table1 [%s] does not have \
	same type as Field2 [%s] in Table2 [%s] \n", f1, t1, f2, t2);
    go_BYE(-1); 
  }
  if ( f1_fld_meta->n_sizeof != f2_fld_meta->n_sizeof ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Append X2 to field for f1
  ofp = fopen(f1_fld_meta->filename, "ab");
  return_if_fopen_failed(ofp, f1_fld_meta->filename, "ab");
  status = rs_mmap(f2_fld_meta->filename, &X2, &nX2, 0); cBYE(status);
  fwrite(X2, nX2, 1, ofp);
  fclose_if_non_null(ofp);
  //--------------------------------------------------------
  // See if there is an auxilary field (nn) to copy
  nn_f1_id = f1_fld_meta->nn_fld_id;
  nn_f2_id = f2_fld_meta->nn_fld_id;
  if ( ( nn_f1_id < 0 ) && ( nn_f2_id < 0 ) ) { 
    /* Nothing to do */
  }
  else if ( ( nn_f1_id < 0 ) && ( nn_f2_id >= 0 ) ) { 
    /* f1 does not have a nn field but f2 does */
    /* Create a file for newly created nn field for f1 */
    status = open_temp_file(&ofp, &nn_opfile, 0); cBYE(status);
    /* Set first nR1 values to true */
    nn_val = TRUE;
    for ( long long i = 0; i < nR1; i++ ) {  // TODO: SPEED THIS UP
      fwrite(&nn_val, sizeof(char), 1, ofp);
    }
    /* Set next nR2 values based on f2 */
    status = rs_mmap(g_fld[nn_f2_id].filename, &Y, &nY, 0); cBYE(status);
    fwrite(Y, nY, 1, ofp);
    fclose_if_non_null(ofp);
    free_if_non_null(nn_opfile);
    rs_munmap(Y, nY);
    /* Add to meta data */
    status = add_aux_fld(t1, f1, nn_opfile, "nn", &nn_f1_id);
    cBYE(status);
    // done in add_aux_fld g_fld[f1_id].nn_fld_id = nn_f1_id;
  }
  else if ( ( nn_f1_id >= 0 ) && ( nn_f2_id < 0 ) ) { 
    /* f1 has a nn field but f2 does not */
    /* Create a file for newly created nn field for f2 */
    status = open_temp_file(&ofp, &nn_opfile, 0); cBYE(status);
    /* Set first nR1 values based on f1 */
    status = rs_mmap(g_fld[nn_f1_id].filename, &Y, &nY, 0); cBYE(status);
    fwrite(Y, nY, 1, ofp);
    /* Set next nR2 values to true */
    nn_val = TRUE;
    for ( long long i = 0; i < nR2; i++ ) {  // TODO: SPEED THIS UP
      fwrite(&nn_val, sizeof(char), 1, ofp);
    }
    fclose_if_non_null(ofp);
    rs_munmap(Y, nY);
    /* Add to meta data */
    status = drop_null_fld(t1, f1); cBYE(status); 
    status = add_aux_fld(t1, f1, nn_opfile, "nn", &nn_f1_id);
    cBYE(status);
    free_if_non_null(nn_opfile);
    // done in add_aux_fld g_fld[f1_id].nn_fld_id = nn_f1_id;

  }
  else { /* Both f1 and f2 have nn fields */
    char *file1 = g_fld[nn_f1_id].filename;
    ofp = fopen(file1, "ab");
    return_if_fopen_failed(ofp, file1, "ab");
    status = rs_mmap(g_fld[nn_f2_id].filename, &Y, &nY, 0); cBYE(status);
    fwrite(Y, nY, 1, ofp);
    fclose_if_non_null(ofp);
  }
  //--------------------------------------------------------
  // See if there is an auxilary field (sz) to copy
  sz_f1_id = f1_fld_meta->sz_fld_id;
  sz_f2_id = f2_fld_meta->sz_fld_id;
  if ( sz_f1_id >= 0 ) { 
    if ( sz_f2_id < 0 ) { go_BYE(-1); }
    chk_range(sz_f1_id, 0, g_n_fld);
    chk_range(sz_f2_id, 0, g_n_fld);

    char *file1 = g_fld[sz_f1_id].filename;
    status = rs_mmap(g_fld[sz_f2_id].filename, &Y, &nY, 0);
    cBYE(status);

    ofp = fopen(file1, "ab");
    return_if_fopen_failed(ofp, file1, "ab");
    fwrite(Y, nY, 1, ofp);
    fclose_if_non_null(ofp);
  }
  //--------------------------------------------------------
 BYE:
  fclose_if_non_null(ofp);
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(sz_opfile);
  return(status);
}
