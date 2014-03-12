#include <math.h>
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
#include "unpack.h"

// last review 9/4/2013


// START FUNC DECL
int
unpack(
       char *tbl,
       char *fin,
       char *str_shifts,
       char *str_nbits,
       char *str_dst_fldtypes,
       char *str_flds
       )
// STOP FUNC DECL
{
  int status = 0;

  char **Y = NULL; int  nY = 0;
  char **Z = NULL; int  nZ = 0;
  char **W = NULL; int  nW = 0;
  char **flds = NULL; int n_flds = 0;
	
  char *fin_X; size_t fin_nX = 0;

  char *Xs[MAX_PACK_FLDS]; size_t nXs[MAX_PACK_FLDS];
  int shifts[MAX_PACK_FLDS];
  int nbits[MAX_PACK_FLDS];
  unsigned long long masks[MAX_PACK_FLDS];
  FLD_REC_TYPE fin_rec, nn_fin_rec;
  FLD_REC_TYPE temp_fld_rec; int temp_fld_id;
  int fin_id, nn_fin_id; 
  FLD_REC_TYPE fld_recs[MAX_PACK_FLDS]; FLD_REC_TYPE nn_fld_recs[MAX_PACK_FLDS];
  int fld_ids[MAX_PACK_FLDS], nn_fld_ids[MAX_PACK_FLDS]; 
  FLD_TYPE dst_fldtypes[MAX_PACK_FLDS];
  TBL_REC_TYPE tbl_rec; int tbl_id; long long nR;
  int filenos[MAX_PACK_FLDS];
  int ddir_ids[MAX_PACK_FLDS];

  // basic checks
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fin == NULL ) || ( *fin == '\0' ) ) { go_BYE(-1); }
  if ( ( str_flds == NULL ) || ( *str_flds == '\0' ) ) { go_BYE(-1); }
  if ( ( str_shifts == NULL ) || ( *str_shifts == '\0' ) ) { go_BYE(-1); }
  if ( ( str_dst_fldtypes == NULL ) || ( *str_dst_fldtypes == '\0' ) ) { go_BYE(-1); }
  //---------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  // get access to input 
  status = is_fld(NULL, tbl_id, fin, &fin_id, &fin_rec, &nn_fin_id,&nn_fin_rec);
  if ( ( fin_rec.fldtype != I8 ) && ( fin_rec.fldtype != I4 ) ) {
    go_BYE(-1);
  }
  status = get_data(fin_rec, &fin_X, &fin_nX, false); cBYE(status);
  // initializations
  for ( int i = 0; i < MAX_PACK_FLDS; i++ ) {
    zero_fld_rec(&(fld_recs[i])); 
    zero_fld_rec(&(nn_fld_recs[i]));
    dst_fldtypes[i] = undef_fldtype;
    Xs[i]           = NULL; 
    nXs[i]          = 0;
    fld_ids[i]      = -1; 
    nn_fld_ids[i]   = -1;
    shifts[i]       = -1;
    nbits[i]       = -1;
    masks[i]        = 0;
    ddir_ids[i]     = -1;
    filenos[i]      = -1;
  }
  // get names of fields to unpack 
  status = explode(str_flds, ':', &flds, &n_flds); cBYE(status);
  if ( ( n_flds < 2 ) || ( n_flds > MAX_NUM_FLDS ) ) { go_BYE(-1); }

  // get shifts for each field 
  status = explode(str_shifts, ':', &Y, &nY); cBYE(status);
  if ( nY != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    status = stoI4(Y[i], &(shifts[i])); cBYE(status);
    if ( shifts[i] < 0 ) { go_BYE(-1); }
  }
  // get nbits for each field 
  status = explode(str_nbits, ':', &W, &nW); cBYE(status);
  if ( nW != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    status = stoI4(W[i], &(nbits[i])); cBYE(status);
    if ( nbits[i] < 1 ) { go_BYE(-1); }
    switch ( fin_rec.fldtype ) { 
      case I4 : if ( nbits[i] >= 32 ) { go_BYE(-1); } break;
      case I8 : if ( nbits[i] >= 64 ) { go_BYE(-1); } break;
      default : go_BYE(-1); break;
    }
  }
  // get fldtypes for each field 
  status = explode(str_dst_fldtypes, ':', &Z, &nZ); cBYE(status);
  if ( nZ != n_flds ) { go_BYE(-1); }
  for ( int i = 0; i < n_flds; i++ ) {
    status = unstr_fldtype(Z[i], &(dst_fldtypes[i])); cBYE(status);
    if ( ( dst_fldtypes[i] == I1 ) || ( dst_fldtypes[i] == I2 ) || 
         ( dst_fldtypes[i] == I4 ) || ( dst_fldtypes[i] == I8 ) ) {
      /* all is well */
    }
    else {
      go_BYE(-1);
    }
    if ( ( fin_rec.fldtype == I4 ) && ( dst_fldtypes[i] == I8 ) ) {
      go_BYE(-1);
    }
    switch ( dst_fldtypes[i] ) { 
      case I1 : if ( nbits[i] >  7 ) { go_BYE(-1); } break; 
      case I2 : if ( nbits[i] > 15 ) { go_BYE(-1); } break; 
      case I4 : if ( nbits[i] > 31 ) { go_BYE(-1); } break; 
      case I8 : if ( nbits[i] > 63 ) { go_BYE(-1); } break; 
      default : go_BYE(-1); break;
    }
    // allocate space for output 
    int fldsz = 0; size_t filesz = 0;
    status = get_fld_sz(dst_fldtypes[i], &fldsz); cBYE(status);
    filesz = fldsz * nR; 
    status = mk_temp_file(filesz, &(ddir_ids[i]), &(filenos[i])); cBYE(status);
    status = q_mmap(ddir_ids[i], filenos[i], &(Xs[i]), &(nXs[i]), true); 
    cBYE(status);
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    masks[i] = two_raised_to((unsigned int)nbits[i]) - 1;
  }
  //----------------------------------------------------------
  // Here starts the core processing 
  unsigned long long *finI8 = (unsigned long long *)fin_X;
  unsigned int       *finI4 = (unsigned int       *)fin_X;
  long long block_size; int nT;
  status = partition(nR, 8192, -1, &block_size, &nT); cBYE(status);

#pragma omp parallel for 
  for ( int tid = 0; tid < nT; tid++ ) {
    long long lb = tid * block_size;
    long long ub = lb  + block_size;
    if ( tid == (nT-1) ) { ub = nR; }
    switch ( fin_rec.fldtype ) {
    case I4 : 
      for ( long long i = lb; i < ub; i++ ) {
	unsigned int inI4 = finI4[i];
	for ( int j = 0; j < n_flds; j++ ) { 
	  char  *tmpI1 = NULL;
	  short *tmpI2 = NULL;
	  int   *tmpI4 = NULL;
	  unsigned int valUI4 = ( inI4 >> shifts[j] )  & masks[j];
	  int valI4 = (int)valUI4;
	  switch ( dst_fldtypes[j] ) {
	  case I1 : 
	    if ( ( valI4 < SCHAR_MIN ) || ( valI4 > SCHAR_MAX ) ) { 
	      fprintf(stderr, "I1: valI4 out of bounds = %d \n", valI4); 
	      status = -1; 
	    }
	    tmpI1    = (char *)Xs[j];
	    tmpI1[i] = (char)valI4;
	    break; 
	  case I2 : 
	    if ( ( valI4 < SHRT_MIN ) || ( valI4 > SHRT_MAX ) ) { 
	      fprintf(stderr, "I2: valI4 out of bounds = %d \n", valI4); 
	      status=-1; 
	    }
	    tmpI2    = (short *)Xs[j];
	    tmpI2    = (short *)Xs[j];
	    tmpI2[i] = (short)valI4;
	    break; 
	  case I4 : 
	    tmpI4    = (int *)Xs[j];
	    tmpI4[i] = (int)valI4;
	    break; 
	  default : 
	    if ( status == 0 ) { WHEREAMI; } status = -1; 
	    break;
	  }
	}
      }
      break;
    case I8 : 
      for ( long long i = lb; i < ub; i++ ) {
	unsigned long long inI8 = finI8[i];
	for ( int j = 0; j < n_flds; j++ ) { 
	  char      *tmpI1 = NULL;
	  short     *tmpI2 = NULL;
	  int       *tmpI4 = NULL;
	  long long *tmpI8 = NULL;
	  unsigned long long valUI8 = ( inI8 >> shifts[j] )  & masks[j];
	  long long valI8 = (long long) valUI8;
	  switch ( dst_fldtypes[j] ) {
	  case I1 : 
	    if ( ( valI8 < SCHAR_MIN ) || ( valI8 > SCHAR_MAX ) ) { 
	      fprintf(stderr, "I1: valI8 out of bounds = %lld \n", valI8); 
	      status = -1; 
	    }
	    tmpI1    = (char *)Xs[j];
	    tmpI1[i] = (char)valI8;
	    break; 
	  case I2 : 
	    if ( ( valI8 < SHRT_MIN ) || ( valI8 > SHRT_MAX ) ) { 
	      fprintf(stderr, "I2: valI8 out of bounds = %lld \n", valI8); 
	      status=-1; 
	    }
	    tmpI2    = (short *)Xs[j];
	    tmpI2[i] = (short)valI8;
	    break; 
	  case I4 : 
	    if ( ( valI8 < INT_MIN ) || ( valI8 > INT_MAX ) ) { 
	      fprintf(stderr, "I4: valI8 out of bounds = %lld \n", valI8); 
	      status=-1; 
	    }
	    tmpI4    = (int *)Xs[j];
	    tmpI4[i] = (int)valI8;
	    break; 
	  case I8 : 
	    tmpI8    = (long long *)Xs[j];
	    tmpI8[i] = (long long)valI8;
	    break; 
	  default : 
	    if ( status == 0 ) { WHEREAMI; } status = -1; 
	    break;
	  }
	}
      }
      break;
    default : 
      if ( status == 0 ) { WHEREAMI; } status = -1; 
      break;

    }
  }
  cBYE(status);
  /* add fields */
  for ( int i = 0; i < n_flds; i++ ) { 
    temp_fld_id = -1; 
    zero_fld_rec(&temp_fld_rec); temp_fld_rec.fldtype = dst_fldtypes[i];
    status = add_fld(tbl_id, flds[i], ddir_ids[i], filenos[i], 
		     &temp_fld_id, &temp_fld_rec);
    cBYE(status);
  }
 BYE:
  for ( int i = 0; i < n_flds; i++ ) {
    rs_munmap(Xs[i], nXs[i]);
    free_if_non_null(Y[i]);
    free_if_non_null(Z[i]);
    free_if_non_null(W[i]);
    free_if_non_null(flds[i]);
  }
  rs_munmap(fin_X, fin_nX);
  free_if_non_null(Y);
  free_if_non_null(Z);
  free_if_non_null(W);
  free_if_non_null(flds);
  return(status);
}
