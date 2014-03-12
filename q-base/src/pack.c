#ifdef IPP
#include "ipp.h"
#include "ippi.h"
#include "ipps.h"
#endif
#ifdef ICC
#include <cilk/cilk.h>
#endif
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "add_fld.h"
#include "get_type_op_fld.h"
#include "mk_file.h"
#include "mk_temp_file.h"
#include "is_tbl.h"
#include "is_fld.h"

// last review 9/4/2013
// START FUNC DECL
int
pack(
     char *tbl,
     char *str_flds,
     char *str_shifts,
     char *str_dst_fldtype,
     char *fout
     )
// STOP FUNC DECL
{
  int status = 0;

  char **Y = NULL; int  nY = 0;
  char **flds = NULL; int n_flds = 0;
	
  char *fout_X; size_t fout_nX = 0;

  char *Xs[MAX_PACK_FLDS]; size_t nXs[MAX_PACK_FLDS];
  int shifts[MAX_PACK_FLDS];
  int fld_ids[MAX_PACK_FLDS]; int nn_fld_ids[MAX_PACK_FLDS];
  FLD_REC_TYPE fld_recs[MAX_PACK_FLDS]; FLD_REC_TYPE nn_fld_recs[MAX_PACK_FLDS];
  FLD_TYPE dst_fldtype;
  TBL_REC_TYPE tbl_rec; int tbl_id; long long nR;
  int ddir_id = -1, fileno = -1;
  int fout_id; FLD_REC_TYPE fout_rec;

  // basic checks
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_shifts == NULL ) || ( *str_shifts == '\0' ) ) { go_BYE(-1); }
  if ( ( str_dst_fldtype == NULL ) || ( *str_dst_fldtype == '\0' ) ) { go_BYE(-1); }
  if ( ( fout == NULL ) || ( *fout == '\0' ) ) { go_BYE(-1); }
  status = unstr_fldtype(str_dst_fldtype, &dst_fldtype); cBYE(status);
  if ( ( dst_fldtype == I4 ) || ( dst_fldtype == I8 ) ) {
    /* all is well */
  }
  else {
    go_BYE(-1);
  }
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  nR = tbl_rec.nR;
  // initializations
  for ( int i = 0; i < MAX_PACK_FLDS; i++ ) {
    Xs[i] = NULL; nXs[i] = 0;
    fld_ids[i] = -1; nn_fld_ids[i] = -1;
    shifts[i] = -1;
    zero_fld_rec(&(fld_recs[i])); zero_fld_rec(&(nn_fld_recs[i]));
  }
  // get names of fields to pack 
  status = explode(str_flds, ':', &flds, &n_flds); cBYE(status);
  if ( ( n_flds < 2 ) || ( n_flds > MAX_NUM_FLDS ) ) { go_BYE(-1); }

  // get shifts for each field 
  status = explode(str_shifts, ':', &Y, &nY); cBYE(status);
  if ( nY != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    status = stoI4(Y[i], &(shifts[i])); cBYE(status);
    if ( shifts[i] < 0 ) { go_BYE(-1); }
  }
  // get access to all inputs 
  for ( int i = 0; i < n_flds; i++ ) {
    if ( strcmp(flds[i], fout) == 0 ) { go_BYE(-1); }
    status = is_fld(NULL, tbl_id, flds[i], &(fld_ids[i]), &(fld_recs[i]), 
		    &(nn_fld_ids[i]), &(nn_fld_recs[i]));
    cBYE(status);
    if ( fld_ids[i] < 0 ) { go_BYE(-1); }
    if ( nn_fld_ids[i] >= 0 ) { go_BYE(-1); }
    // check field type is okay
    switch ( fld_recs[i].fldtype ) { 
    case I1 : case I2 : case I4 : case I8 : break;
    default : go_BYE(-1); break;
    }
    status = get_data(fld_recs[i], &(Xs[i]), &(nXs[i]), false); cBYE(status);
    if ( ( dst_fldtype == I4 ) && ( fld_recs[i].fldtype == I8 ) ) {
      go_BYE(-1);
    }
  }
  // allocate space for output 
  int fldsz = 0; size_t filesz = 0;
  status = get_fld_sz(dst_fldtype, &fldsz); cBYE(status);
  filesz = fldsz * nR; 
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &fout_X, &fout_nX, true); cBYE(status);

  char      *I1ptr = NULL;
  short     *I2ptr = NULL;
  int       *I4ptr = NULL;
  long long *I8ptr = NULL;
  switch ( dst_fldtype ) {
  case I4 :
// #include "incl_pack_I4.c"
#pragma cilk grainsize = 4096
    for ( long long i = 0; i < nR; i++ ) {
      int *fout = (int *)fout_X;
      int outval = 0;
      if ( status < 0 ) { continue; }
      for ( int j = 0; j < n_flds; j++ ) { 
	int thisval = 0;
	switch ( fld_recs[j].fldtype ) {
	case I1 : 
	  I1ptr = (char      *)Xs[j]; 
	  thisval = I1ptr[i]; 
	  break; 
	case I2 : 
	  I2ptr = (short     *)Xs[j]; 
	  thisval = I2ptr[i]; 
	  break; 
	case I4 : 
	  I4ptr = (int       *)Xs[j]; 
	  thisval = I4ptr[i]; 
	  break; 
	case I8 : 
	  I8ptr = (long long *)Xs[j]; 
	  thisval = I8ptr[i]; 
	  break; 
	default : break;
	}
        if ( thisval < 0 ) { status = -1; continue; }
	thisval = thisval << shifts[j];
	outval = outval | thisval;
      }
      fout[i] = outval;
    }
    break;
  case I8 :
// #include "incl_pack_I8.c"
#pragma cilk grainsize = 4096
    for ( long long i = 0; i < nR; i++ ) {
      long long *fout = (long long *)fout_X;
      long long outval = 0;
      if ( status < 0 ) { continue; }
      for ( int j = 0; j < n_flds; j++ ) { 
	long long thisval = 0;
	switch ( fld_recs[j].fldtype ) {
	case I1 : 
	  I1ptr = (char      *)Xs[j]; 
	  thisval = I1ptr[i]; 
	  break; 
	case I2 : 
	  I2ptr = (short     *)Xs[j]; 
	  thisval = I2ptr[i]; 
	  break; 
	case I4 : 
	  I4ptr = (int       *)Xs[j]; 
	  thisval = I4ptr[i]; 
	  break; 
	case I8 : 
	  I8ptr = (long long *)Xs[j]; 
	  thisval = I8ptr[i]; 
	  break; 
	default : break;
	}
        if ( thisval < 0 ) { status = -1; continue; }
	thisval = thisval << shifts[j];
	outval = outval | thisval;
      }
      fout[i] = outval;
    }
    break;
  default :
    go_BYE(-1);
    break;
  }
  cBYE(status);
  /* add fields */
  zero_fld_rec(&fout_rec); fout_rec.fldtype = dst_fldtype;
  status = add_fld(tbl_id, fout, ddir_id, fileno, &fout_id, &fout_rec);
  cBYE(status);
 BYE:
  for ( int i = 0; i < n_flds; i++ ) {
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(Y[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(fout_X, fout_nX);
  free_if_non_null(Y);
  free_if_non_null(flds);
  return(status);
}
