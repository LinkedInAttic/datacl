#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "mk_temp_file.h"
#include "copy_fld.h"
#include "meta_globals.h"

#define UPPER_TRIANGULAR_MINUS_DIAGONAL 100
#define UPPER_TRIANGULAR                200
#define COMPLETE                        300
//---------------------------------------------------------------
// START FUNC DECL
int 
crossprod(
	  char *t1,
	  char *f1,
	  char *f2,
	  char *t2,
	  char *t2f1,
	  char *t2f2,
	  char *taux,
	  char *batch_sz,
	  char *str_mode
	  )
// STOP FUNC DECL
{
  int status = 0;
  long long nR1, nR2, nRaux = 0;
  int t1_id, t2_id, taux_id;
  TBL_REC_TYPE t1_rec, t2_rec, taux_rec;

  FLD_REC_TYPE t1f1_rec, nn_t1f1_rec; int t1f1_id, nn_t1f1_id;
  FLD_REC_TYPE t1f2_rec, nn_t1f2_rec; int t1f2_id, nn_t1f2_id;

  FLD_REC_TYPE batch_sz_rec, nn_batch_sz_rec; int batch_sz_id, nn_batch_sz_id;

  FLD_REC_TYPE t2f1_rec; int t2f1_id;
  FLD_REC_TYPE t2f2_rec; int t2f2_id;

  char *t1f1_X = NULL;   size_t t1f1_nX = 0;
  char *t1f2_X = NULL;   size_t t1f2_nX = 0;
  char *batch_sz_X = NULL;   size_t batch_sz_nX = 0;
  char *t2f1_X = NULL; size_t t2f1_nX = 0;
  char *t2f2_X = NULL; size_t t2f2_nX = 0;

  int *batchszI4 = NULL;
  int t2f1_fileno = -1, t2f2_fileno = -1;
  int t2f1_ddir_id = -1, t2f2_ddir_id = -1; size_t filesz; int fldsz;

  bool is_aux; int mode;

  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); } 
  if ( ( taux == NULL ) || ( *taux == '\0' ) ) { 
    if ( ( batch_sz != NULL ) && ( *batch_sz != '\0' ) ) { go_BYE(-1); }
    is_aux = false;
  }
  else {
    if ( strcmp(taux, t2) == 0 ) { go_BYE(-1); } 
    if ( strcmp(taux, t1) == 0 ) { go_BYE(-1); } 
    if ( ( batch_sz == NULL ) || ( *batch_sz == '\0' ) ) { go_BYE(-1); }
    is_aux = true;
  }

  if ( ( t2f1 == NULL ) || ( *t2f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2f2 == NULL ) || ( *t2f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t2f1, t2f2) == 0 ) { go_BYE(-1); }
  if ( ( str_mode == NULL ) || ( *str_mode == '\0' ) )  {
    mode = COMPLETE;
  }
  else {
    if ( strcasecmp(str_mode, "upper_triangular_minus_diagonal") == 0 ) {
      mode = UPPER_TRIANGULAR_MINUS_DIAGONAL;
    }
    else if ( strcasecmp(str_mode, "upper_triangular") == 0 ) {
      mode = UPPER_TRIANGULAR;
    }
    else if ( strcasecmp(str_mode, "complete") == 0 ) {
      mode = COMPLETE;
    }
    else {
      go_BYE(-1);
    }
  }
  //----------------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR; 
  if ( nR1 <= 1 ) { go_BYE(-1); }

  status = is_fld(NULL, t1_id, f1, &t1f1_id, &t1f1_rec, &nn_t1f1_id, &nn_t1f1_rec);
  if ( t1f1_id < 0 ) { go_BYE(-1); }
  if ( nn_t1f1_id >= 0 ) { go_BYE(-1); }
  if ( t1f1_rec.fldtype != I4 ) { go_BYE(-1); }
  status = get_data(t1f1_rec, &t1f1_X, &t1f1_nX, false); cBYE(status);
  //--------------------------------------------------------
  if ( strcmp(f2, f1) == 0 ) {
    t1f2_X = t1f1_X;
  }
  else {
    status = is_fld(NULL, t1_id, f2, &t1f2_id, &t1f2_rec, &nn_t1f2_id, &nn_t1f2_rec);
    if ( t1f2_id < 0 ) { go_BYE(-1); }
    if ( nn_t1f2_id >= 0 ) { go_BYE(-1); }
    if ( t1f2_rec.fldtype != I4 ) { go_BYE(-1); }
    if ( t1f2_rec.fldtype != t1f1_rec.fldtype ) { go_BYE(-1); }
    status = get_data(t1f2_rec, &t1f2_X, &t1f2_nX, false); cBYE(status);
  }
  //--------------------------------------------------------
  status = del_tbl(t2, -1); 
  //--------------------------------------------------------
  if ( is_aux == true ) { 
    status = is_tbl(taux, &taux_id, &taux_rec); cBYE(status);
    if ( taux_id <  0 ) { go_BYE(-1); }
    nRaux = taux_rec.nR; 

    status = is_fld(NULL, taux_id, batch_sz, &batch_sz_id, &batch_sz_rec, &nn_batch_sz_id, &nn_batch_sz_rec);
    if ( batch_sz_id <  0 ) { go_BYE(-1); }
    status = get_data(batch_sz_rec, &batch_sz_X, &batch_sz_nX, false); 
    cBYE(status);
    if ( nn_batch_sz_id >= 0 ) { go_BYE(-1); }
    if ( batch_sz_rec.fldtype != I4 ) { go_BYE(-1); }

    batchszI4 = (int *)batch_sz_X;
    long long sum = 0;
    nR2 = 0; 
    for ( int i = 0;  i < nRaux; i++ ) {
      int sz = batchszI4[i];
      if ( sz <= 1 ) { go_BYE(-1); }
      sum += sz;
      switch ( mode ) {
      case COMPLETE                        : nR2 += (sz * sz ); break;
      case UPPER_TRIANGULAR                : nR2 += (sz*(sz+1))/2; break;
      case UPPER_TRIANGULAR_MINUS_DIAGONAL : nR2 += ((sz-1)*sz)/2 ; break;
      default : go_BYE(-1); break;
      }
    }
    if ( sum != nR1 ) { go_BYE(-1); }
    if ( nR2 > (nR1 * nR1) ) { go_BYE(-1); }
  }
  else {
    switch ( mode ) {
    case COMPLETE                        : nR2 = nR1 * nR1; break;
    case UPPER_TRIANGULAR                : nR2 = (nR1*(nR1+1))/2; break;
    case UPPER_TRIANGULAR_MINUS_DIAGONAL : nR2 = ((nR1-1)*nR1)/2 ; break;
    default : go_BYE(-1); break;
    }
  }
  // allocate space for output
  status = get_fld_sz(t1f1_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR2;

  status = mk_temp_file(filesz, &t2f1_ddir_id, &t2f1_fileno); cBYE(status);
  status = q_mmap(t2f1_ddir_id, t2f1_fileno, &t2f1_X, &t2f1_nX, true); cBYE(status);

  status = mk_temp_file(filesz, &t2f2_ddir_id, &t2f2_fileno); cBYE(status);
  status = q_mmap(t2f2_ddir_id, t2f2_fileno, &t2f2_X, &t2f2_nX, true); cBYE(status);

  char buffer[32];
  sprintf(buffer, "%lld", nR2);
  status = add_tbl(t2, buffer, &t2_id, &t2_rec); cBYE(status);

  /* create the new fields */
  int t1f1I4, t1f2I4;
  int *t2f1I4ptr = (int *)t2f1_X;
  int *t2f2I4ptr = (int *)t2f2_X;
  int *t1f1I4ptr = (int *)t1f1_X;
  int *t1f2I4ptr = (int *)t1f2_X;
  long long chk_nR2 = 0;

  if ( is_aux == true ) { 
    int lbidx = 0, ubidx;
    for ( int grp = 0; grp < nRaux; grp++ ) { 
      ubidx = lbidx + batchszI4[grp];
      for ( int j = lbidx; j < ubidx; j++ ) {
	t1f1I4 = t1f1I4ptr[j];
	int start;
	switch ( mode ) {
	case COMPLETE : start = lbidx; break; 
	case UPPER_TRIANGULAR : start = j; break; 
	case UPPER_TRIANGULAR_MINUS_DIAGONAL : start = j+1; break;
	default : go_BYE(-1); break;
	}
	for ( int k = start; k < ubidx; k++ ) { // TODO P0: Document the +1
	  t1f2I4 = t1f2I4ptr[k];
	  t2f1I4ptr[chk_nR2] = t1f1I4;
	  t2f2I4ptr[chk_nR2] = t1f2I4;
	  chk_nR2++;
	}
      }
      lbidx = ubidx;
    }
  }
  else {
    for ( int j = 0; j < nR1; j++ ) { 
      t1f1I4 = t1f1I4ptr[j];
      int start;
      switch ( mode ) {
      case COMPLETE : start = 0; break; 
      case UPPER_TRIANGULAR : start = j; break; 
      case UPPER_TRIANGULAR_MINUS_DIAGONAL : start = j+1; break;
      default : go_BYE(-1); break;
      }
      for ( int k = start; k < nR1; k++ ) { 
        t1f2I4 = t1f2I4ptr[k];
	t2f1I4ptr[chk_nR2] = t1f1I4;
	t2f2I4ptr[chk_nR2] = t1f2I4;
	chk_nR2++;
      }
    }
  }
  if ( nR2 != chk_nR2 ) { go_BYE(-1); }
  /* add fields */
  zero_fld_rec(&t2f1_rec); t2f1_rec.fldtype = t1f1_rec.fldtype;
  status = add_fld(t2_id, t2f1, t2f1_ddir_id, t2f1_fileno, &t2f1_id, &t2f1_rec);
  cBYE(status);

  zero_fld_rec(&t2f2_rec); t2f2_rec.fldtype = t1f2_rec.fldtype;
  status = add_fld(t2_id, t2f2, t2f2_ddir_id, t2f2_fileno, &t2f2_id, &t2f2_rec);
  cBYE(status);

 BYE:
  rs_munmap(t1f1_X, t1f1_nX);
  if ( ( f1 != NULL ) && ( f2 != NULL ) ) {
    if ( strcmp(f2, f1) != 0 ) {
      rs_munmap(t1f2_X, t1f2_nX);
    }
  }
  rs_munmap(t2f1_X, t2f1_nX);
  rs_munmap(t2f2_X, t2f2_nX);
  return status ;
}
