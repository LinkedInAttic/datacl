#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "dbauxil.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "is_tbl.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "set_meta.h"
#include "is_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

// last review 9/4/2013

// START FUNC DECL
int
stride_I4(
    int *f1,
    long long nR1,
    long long start, 
    long long stride, 
    int *f2,
    long long nR2
    )
// STOP FUNC DECL
{
  int status = 0;
  long long f1_idx = start;
  for ( long long i = 0; i < nR2; i++ ) {
    if ( ( f1_idx < 0 ) || ( f1_idx >= nR1 ) )  { go_BYE(-1); }
    f2[i] = f1[f1_idx];
    f1_idx += stride;
  }
BYE:
  return status ;
}
//---------------------------------------------------------------
// START FUNC DECL
int
stride_I8(
    long long *f1,
    long long nR1,
    long long start, 
    long long stride, 
    long long *f2,
    long long nR2
    )
// STOP FUNC DECL
{
  int status = 0;
  long long f1_idx = start;
  for ( long long i = 0; i < nR2; i++ ) {
    if ( ( f1_idx < 0 ) || ( f1_idx >= nR1 ) )  { go_BYE(-1); }
    f2[i] = f1[f1_idx];
    f1_idx += stride;
  }
BYE:
  return status ;
}
//---------------------------------------------------------------
// START FUNC DECL
int 
ext_stride(
	   const char *t1,
	   const char *f1,
	   char *str_start,
	   char *str_stride,
	   const char *t2,
	   const char *f2,
	   char *str_nR2
    )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  long long nR1, nR2;
  TBL_REC_TYPE t1_rec; int t1_id; 
  TBL_REC_TYPE t2_rec; int t2_id; 
  FLD_REC_TYPE f1_rec; int f1_id; 
  FLD_REC_TYPE nn_f1_rec; int nn_f1_id; 
  FLD_REC_TYPE f2_rec; int f2_id; 
  long long start, stride; 
  int fileno = -1, ddir_id = INT_MAX; 
  int fldsz = INT_MAX; size_t filesz = 0;
  int       *f1I4 = NULL,  *f2I4 = NULL;
  long long *f1I8 = NULL,  *f2I8 = NULL;

  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  status = stoI8(str_start,  &start); cBYE(status);
  status = stoI8(str_stride, &stride); cBYE(status);
  nR2    = stoI8(str_nR2,    &nR2); cBYE(status);
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  nR1 = t1_rec.nR;
  status = is_fld(t1, -1, f1, &f1_id, &f1_rec, &nn_f1_id, &nn_f1_rec);
  cBYE(status);
  status = get_data(f1_rec, &f1_X, &f1_nX, false); cBYE(status);
  if ( nn_f1_id >= 0 ) { go_BYE(-1); }
  status = del_tbl(t2, -1); cBYE(status); 
  if ( ( f1_rec.fldtype == I4 ) || ( f1_rec.fldtype == I8 ) ) {
    /* all is well */
  }
  else {
    go_BYE(-1);
  }

  if ( ( start  <  0 ) || ( start  >= nR1 ) ) { go_BYE(-1); }
  if ( ( stride <= 1 ) || ( stride >= nR1 ) ) { go_BYE(-1); }
  if ( nR2 < 1 ) { go_BYE(-1); }

  // reduce nR2 if necessary
  while ( start + (stride * nR2) > nR1 ) { nR2--; }
  if ( nR2 < 1 ) { go_BYE(-1); }
  //--------------------------------------------------------
  // allocate space for output
  status = get_fld_sz(f1_rec.fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR2; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &f2_X, &f2_nX, true); cBYE(status);

  switch ( f1_rec.fldtype ) { 
    case I4 : 
      f1I4 = (int *)f1_X;
      f2I4 = (int *)f2_X;
      status = stride_I4(f1I4, nR1, start, stride, f2I4, nR2); cBYE(status);
      break;
    case I8 : 
      f1I8 = (long long *)f1_X;
      f2I8 = (long long *)f2_X;
      status = stride_I8(f1I8, nR1, start, stride, f2I8, nR2); cBYE(status);
      break;
    default :
      go_BYE(-1);
      break;
  }
  cBYE(status);

  status = add_tbl(t2, str_nR2, &t2_id, &t2_rec); cBYE(status);
  zero_fld_rec(&f2_rec); f2_rec.fldtype = f1_rec.fldtype;
  status = add_fld(t2_id, f2, ddir_id, fileno, &f2_id, &f2_rec);
  cBYE(status);

 BYE:
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  return status ;
}
