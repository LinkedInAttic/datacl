#include <stdlib.h>
#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "permute_I4.h"
#include "subsample.h"

// last review 9/10/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
subsample(
	  char *t1,
	  char *f1,
	  char *str_nR2,
	  char *t2,
	  char *f2
	  )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE f1_rec, f2_rec;
  FLD_REC_TYPE nn_f1_rec;
  long long nR1, nR2; 
  int t1_id = INT_MIN, t2_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN;
  int nn_f1_id = INT_MIN;
  int fldsz = INT_MAX; size_t filesz = 0;
  int ddir_id = -1, fileno = -1; 
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  status = stoI8(str_nR2, &nR2); cBYE(status); 
  if ( nR2 <= 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  if ( nR1 <= nR2 ) { go_BYE(-1); }
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec); 
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  // Not implemented for following cases 
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  switch ( f1_rec.fldtype ) { 
    case I4 : case I8 : break;
    default : go_BYE(-1); break;
  }
  //---------------------------------------------
  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
  filesz = nR2 * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f2_X, &f2_nX, 1);  cBYE(status);

  long long block_size_2; int nT;
  status = partition(nR2, 1024, -1, &block_size_2, &nT); cBYE(status);
  long long block_size_1 = (nR1 / nT);
  int ratio = nR1 / nR2; if ( ratio == 1 ) { ratio++; }

  for ( int tid = 0; tid < nT; tid++ ) {
    if ( status < 0 ) { continue; }
    int *idxs = NULL;
    long long lb1 = tid * block_size_1; /* input lb */
    long long ub1 = lb1 + block_size_1; /* input ub */
    if ( ub1 > nR1 ) { ub1 = nR1; } /* corner case */
    if ( ub1 <= lb1 ) { status = -1; continue; }
    long long range1 = ub1 - lb1;
    if ( range1 >= 16*1048576 ) { status = -1; continue; }
    idxs = malloc(range1 * sizeof(int));
    if ( idxs == NULL ) { status = -1; continue; }
    for ( int i = 0; i < range1; i++ ) {
      idxs[i] = i;
    }
    status = permute_I4(idxs, range1); if ( status < 0 ) { continue; }

    long long lb2 = tid * block_size_2; /* output lb */
    long long ub2 = lb2 + block_size_2; /* output ub */
    if ( tid == (nT-1) ) { ub2 = nR2; } /* corner case */
    long long range2 = ub2 - lb2;

    int       *I4f1 = NULL, *I4f2 = NULL;
    long long *I8f1 = NULL, *I8f2 = NULL;
    I4f1 = (int       *)f1_X; I4f1 += lb1; 
    I4f2 = (int       *)f2_X; I4f2 += lb2;
    I8f1 = (long long *)f1_X; I8f1 += lb1; 
    I8f2 = (long long *)f2_X; I8f2 += lb2;
    switch ( f1_rec.fldtype ) { 
      case I4 : 
	for ( int i = 0; i < range2; i++ ) { I4f2[i] = I4f1[idxs[i]]; }
	break;
      case I8 : 
	for ( int i = 0; i < range2; i++ ) { I8f2[i] = I8f1[idxs[i]]; }
	break;
      default : 
	if ( status == 0 ) { WHEREAMI; } status = -1; continue; 
	break;
    }
    free_if_non_null(idxs);
  }
  cBYE(status);
  //--- Add to t2
  status  = del_tbl(t2, -1); cBYE(status);
  char strbuf[32];
  sprintf(strbuf, "%lld", nR2);
  status = add_tbl(t2, strbuf, &t2_id, &t2_rec); cBYE(status);
  zero_fld_rec(&f2_rec); f2_rec.fldtype = f1_rec.fldtype; 
  status = add_fld(t2_id, f2, ddir_id, fileno, &f2_id, &f2_rec); cBYE(status);
 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return(status);
}
