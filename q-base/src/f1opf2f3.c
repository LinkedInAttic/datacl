#include <limits.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "sort.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f1opf2f3.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

// last review 9/10/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2f3(
	 char *tbl,
	 char *f1,
	 char *op,
	 char *f2,
	 char *f3
	 )
// STOP FUNC DECL
{
  int status = 0;
  int fldsz = 0; size_t filesz = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;
  FLD_REC_TYPE f1_rec, nn_f1_rec;
  FLD_REC_TYPE f2_rec;
  FLD_REC_TYPE f3_rec;
  long long nR; 
  TBL_REC_TYPE tbl_rec;
  int tbl_id = INT_MIN; 
  int f1_id = INT_MIN, nn_f1_id = INT_MIN;
  int f2_id = INT_MIN;
  int f3_id = INT_MIN;
  int f2_ddir_id = -1, f2_fileno = -1;
  int f3_ddir_id = -1, f3_fileno = -1;
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( ( op == NULL ) || ( *op == '\0' ) ) { go_BYE(-1); }
  cBYE(status);
  if ( strcmp(f1, f2) == 0 ) { go_BYE(-1); }
  if ( strcmp(f2, f3) == 0 ) { go_BYE(-1); }
  if ( strcmp(f3, f1) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  chk_range(tbl_id, 0, g_n_tbl);
  nR = g_tbls[tbl_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  if ( f1_rec.fldtype != I8 ) { go_BYE(-1); }
  //--------------------------------------------------------
  /* Create space for output */
  status = get_fld_sz(I4, &fldsz); cBYE(status);
  filesz = fldsz * nR;

  status = mk_temp_file(filesz, &f2_ddir_id, &f2_fileno);
  status = q_mmap(f2_ddir_id, f2_fileno, &f2_X, &f2_nX, 1); cBYE(status);

  status = mk_temp_file(filesz, &f3_ddir_id, &f3_fileno);
  status = q_mmap(f3_ddir_id, f3_fileno, &f3_X, &f3_nX, 1); cBYE(status);
  /*-------------------------------------------------------- */
  if ( strcmp(op, "unconcat") == 0 ) { 
    unsigned long long *f1UI8 = (unsigned long long *)f1_X;
    unsigned int *f2UI4 = (unsigned int *)f2_X;
    unsigned int *f3UI4 = (unsigned int *)f3_X;
    for ( long long i = 0; i < nR; i++ ) { 
      long long valf1UI8 = f1UI8[i];
      f2UI4[i] = (unsigned int)(valf1UI8 >> 32);
      f3UI4[i] = (unsigned int)(valf1UI8 & 0xFFFFFFFF);
    }
    zero_fld_rec(&f2_rec); f2_rec.fldtype = I4; 
    status = add_fld(tbl_id, f2, f2_ddir_id, f2_fileno, &f2_id, &f2_rec); cBYE(status);
    zero_fld_rec(&f3_rec); f3_rec.fldtype = I4; 
    status = add_fld(tbl_id, f3, f3_ddir_id, f3_fileno, &f3_id, &f3_rec); cBYE(status);
  }
  else { 
    fprintf(stderr, "Invalid op = [%s] \n", op);
    go_BYE(-1);
  }
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f3_X, f3_nX);
  return(status);
}
