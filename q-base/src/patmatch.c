#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "is_fld.h"
#include "is_tbl.h"
#include "add_fld.h"
#include "aux_meta.h"
#include "mk_temp_file.h"
#include "patmatch.h"
#include "patmatch_I1.h"
#include "patmatch_I2.h"
#include "patmatch_I4.h"

// START FUNC DECL
int
patmatch(
	 char *t1,
	 char *f1, /* value to match */
	 char *brk_fld, /* session break field */
	 char *out_fld, /* condition field to be created */
	 char *tp, /* table that identifies pattern */
	 char *pat_fld, /* patfld[i] = ith member of pattern */
	 char *lb_fld, /* lb[i] = min number of times patfld[i] should occur */
	 char *ub_fld /* ub[i] = max number of times patfld[i] should occur */
	 )
// STOP FUNC DECL
{
  int status = 0;
  long long nR1 = 0, nR2 = 0;
  int t1_id = -1, tp_id = -1; 
  TBL_REC_TYPE t1_rec, tp_rec;
  int f1_id = -1, brk_id = -1, out_id = -1, nn_id = -1; 
  int lb_id = -1, ub_id = -1, pat_id = -1;
  FLD_REC_TYPE f1_rec, brk_rec, out_rec, nn_rec;
  FLD_REC_TYPE lb_rec, ub_rec, pat_rec;
  char *f1_X  = NULL; size_t f1_nX  = 0;
  char *brk_X = NULL; size_t brk_nX = 0;
  char *out_X = NULL; size_t out_nX = 0;
  char *lb_X  = NULL; size_t lb_nX  = 0;
  char *ub_X  = NULL; size_t ub_nX  = 0;
  char *pat_X = NULL; size_t pat_nX = 0;
  long long filesz = 0; int fldsz = 0; int ddir_id; int fileno;

  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( out_fld == NULL ) || ( *out_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( tp == NULL ) || ( *tp == '\0' ) ) { go_BYE(-1); }
  if ( ( lb_fld == NULL ) || ( *lb_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( ub_fld == NULL ) || ( *ub_fld == '\0' ) ) { go_BYE(-1); }
  if ( ( pat_fld == NULL ) || ( *pat_fld == '\0' ) ) { go_BYE(-1); }

  if ( strcmp(t1, tp) == 0 ) { go_BYE(-1); }
  if ( strcmp(f1, out_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_fld, ub_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(lb_fld, pat_fld) == 0 ) { go_BYE(-1); }
  if ( strcmp(ub_fld, pat_fld) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status); 
  if ( t1_id < 0 ) { go_BYE(-1); }
  nR1 = t1_rec.nR;
  //--------------------------------------------------------
  status = is_fld(NULL, t1_id, f1, &f1_id, &f1_rec, &nn_id, &nn_rec);
  cBYE(status);
  if ( f1_id < 0 ) { go_BYE(-1); }
  if ( nn_id >= 0 ) { go_BYE(-1); }
  status = get_data(f1_rec, &f1_X, &f1_nX, 0); cBYE(status);
  switch ( f1_rec.fldtype ) { 
  case I1 : case I2: case I4 : break;
  default : go_BYE(-1); break;
  }
  //--------------------------------------------------------
  if ( ( brk_fld != NULL ) && ( *brk_fld != '\0' ) )  {
    status = is_fld(NULL, t1_id, brk_fld, &brk_id, &brk_rec, &nn_id, &nn_rec);
    cBYE(status);
    if ( brk_id < 0 ) { go_BYE(-1); }
    if ( nn_id >= 0 ) { go_BYE(-1); }
    status = get_data(brk_rec, &brk_X, &brk_nX, 0); cBYE(status);
    if ( brk_rec.fldtype != I1 ) { go_BYE(-1); }
  }
  //--------------------------------------------------------
  status = is_tbl(tp, &tp_id, &tp_rec); cBYE(status); 
  if ( tp_id < 0 ) { go_BYE(-1); }
  nR2 = tp_rec.nR;
  /* Current assumption is that pattern has length >= 2 */
  if ( nR2 <= 1 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_fld(NULL, tp_id, lb_fld, &lb_id, &lb_rec, &nn_id, &nn_rec);
  cBYE(status);
  if ( lb_id < 0 ) { go_BYE(-1); }
  if ( nn_id >= 0 ) { go_BYE(-1); }
  status = get_data(lb_rec, &lb_X, &lb_nX, 0); cBYE(status);
  if ( lb_rec.fldtype != I4 ) { go_BYE(-1); }
  int *lbI4 = (int *)lb_X;
  //--------------------------------------------------------
  status = is_fld(NULL, tp_id, ub_fld, &ub_id, &ub_rec, &nn_id, &nn_rec);
  cBYE(status);
  if ( ub_id < 0 ) { go_BYE(-1); }
  if ( nn_id >= 0 ) { go_BYE(-1); }
  status = get_data(ub_rec, &ub_X, &ub_nX, 0); cBYE(status);
  if ( ub_rec.fldtype != I4 ) { go_BYE(-1); }
  int *ubI4 = (int *)ub_X;
  //--------------------------------------------------------
  status = is_fld(NULL, tp_id, pat_fld, &pat_id, &pat_rec, &nn_id, &nn_rec);
  cBYE(status);
  if ( pat_id < 0 ) { go_BYE(-1); }
  if ( nn_id >= 0 ) { go_BYE(-1); }
  status = get_data(pat_rec, &pat_X, &pat_nX, 0); cBYE(status);
  if ( pat_rec.fldtype != f1_rec.fldtype ) { go_BYE(-1); }
  //--------------------------------------------------------
  // Quick checks on pattern
  for ( int i = 0; i < nR2; i++ ) { 
    if ( lbI4[i] > ubI4[i] ) { go_BYE(-1); }
    if ( lbI4[i] < 1 ) { go_BYE(-1); }
  }
  //-----------------------------------------------------------
  status = get_fld_sz(I1, &fldsz); cBYE(status);
  filesz = nR1 * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &out_X, &out_nX, 1); cBYE(status);
  //-----------------------------------------------------------
  switch ( f1_rec.fldtype ) { 
      case I1 : 
      status = inner_patmatch_I1((char *)f1_X, brk_X, out_X, nR1, (char *)pat_X, lbI4, ubI4, nR2);
      break;
      case I2 : 
      status = inner_patmatch_I2((short *)f1_X, brk_X, out_X, nR1, (short *)pat_X, lbI4, ubI4, nR2);
      break;
  case I4 : 
    status = inner_patmatch_I4((int *)f1_X, brk_X, out_X, nR1, (int *)pat_X, lbI4, ubI4, nR2);
    break;
  default : 
    go_BYE(-1);
    break;
  }
  cBYE(status);
  zero_fld_rec(&out_rec); out_rec.fldtype = I1;
  status = add_fld(t1_id, out_fld, ddir_id, fileno, &out_id, &out_rec);
  cBYE(status);

 BYE:
  rs_munmap(f1_X,  f1_nX  );
  rs_munmap(brk_X, brk_nX );
  rs_munmap(out_X, out_nX );
  rs_munmap(lb_X,  lb_nX  );
  rs_munmap(ub_X,  ub_nX  );
  rs_munmap(pat_X, pat_nX );
  return(status);
}
