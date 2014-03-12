#include "qtypes.h"
#include "mmap.h"
#include "add_fld.h"
#include "dbauxil.h"
#include "auxil.h"
#include "add_tbl.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "f_to_s.h"
#include "aux_meta.h"
#include "add_aux_fld.h"
#include "mk_temp_file.h"
#include "meta_globals.h"

// last review 9/7/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
copy_fld(
	 char *t1,
	 char *f1,
	 char *cfld,
	 char *t2
	 )
// STOP FUNC DECL
{
  int status = 0;
  char *X = NULL; size_t nX = 0; 
  char *nnopX = NULL;  size_t n_nnopX = 0; 
  char *opX = NULL;  size_t n_opX = 0; 
  char *nnX = NULL; size_t n_nnX = 0; 
  char *cfldX = NULL; size_t n_cfldX = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  FLD_REC_TYPE t1f1_rec, nn_t1f1_rec;
  FLD_REC_TYPE t2f1_rec, nn_t2f1_rec;
  FLD_REC_TYPE cfld_rec, nn_cfld_rec;
  long long nR1 = LLONG_MIN, exp_nR2 = LLONG_MIN, nR2  = LLONG_MIN;
  long long lb = LLONG_MIN, ub = LLONG_MAX;
  int t1_id = INT_MIN, t2_id = INT_MIN; 
  int t1f1_id = INT_MIN, t2f1_id = INT_MIN;
  int cfld_id = INT_MIN, nn_cfld_id = INT_MIN; 
  int nn_t1f1_id = INT_MIN, nn_t2f1_id = INT_MIN;
  bool is_null_val_in_op = false;
#define SZ_RSLT_BUF 32
  char buf1[SZ_RSLT_BUF], buf2[SZ_RSLT_BUF];
  bool is_cfld_a_range = false;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t2) == 0 ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbls[t1_id].nR;

  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  if ( t2_id >= 0 ) {
    nR2 = g_tbls[t2_id].nR;
  }

  status = is_fld(NULL, t1_id, f1, &t1f1_id, &t1f1_rec, 
		  &nn_t1f1_id, &nn_t1f1_rec); cBYE(status);
  chk_range(t1f1_id, 0, g_n_fld);
  status = get_data(t1f1_rec, &X, &nX, false); cBYE(status);
  switch ( t1f1_rec.fldtype ) { 
    case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : break;
    default : go_BYE(-1); break;
  }

  if ( ( cfld != NULL ) && ( *cfld != '\0' ) ) {
    /* See if cfld is a range or a boolean field */
    for ( char *cptr = cfld; *cptr != '\0'; cptr++ ) { 
      if ( *cptr == ':' ) {
	is_cfld_a_range = true;
	break;
      }
    }
    if ( is_cfld_a_range == true ) { 
      status = read_nth_val(cfld, ":", 0, buf1, SZ_RSLT_BUF);
      status = stoI8(buf1, &lb); cBYE(status);
      status = read_nth_val(cfld, ":", 1, buf1, SZ_RSLT_BUF);
      status = stoI8(buf1, &ub); cBYE(status);
      exp_nR2 = ub - lb;
      if ( exp_nR2 <= 0 ) { go_BYE(-1); }
    }
    else {
      status = is_fld(NULL, t1_id, cfld, &cfld_id, &cfld_rec, 
		      &nn_cfld_id, &nn_cfld_rec); cBYE(status);
      chk_range(cfld_id, 0, g_n_fld);
      status = get_data(cfld_rec, &cfldX, &n_cfldX, 0); cBYE(status);
      if ( nn_cfld_id >= 0 ) { go_BYE(-1); }
      if ( ( cfld_rec.fldtype != I1 ) && ( cfld_rec.fldtype != B ) ) {
	go_BYE(-1); 
      }
      if ( cfld_rec.is_sum_nn == true ) { 
	exp_nR2 = cfld_rec.sumval.sumI8;
      }
      else {
	status = f_to_s(t1, cfld, "sum", buf1, SZ_RSLT_BUF); cBYE(status);
	status = read_nth_val(buf1, ":", 0, buf2, SZ_RSLT_BUF);
	status = stoI8(buf2, &exp_nR2); cBYE(status);
      }
    }
  }
  else {
    exp_nR2 = nR1;
  }
  if ( t2_id >= 0 ) {
    if ( exp_nR2 != nR2 ) { 
      fprintf(stderr, "Incompatible number of rows %lld --> %lld \n",
	      exp_nR2, nR2);
      go_BYE(-1); 
    }
  }
  else  {
    nR2 = exp_nR2;
    sprintf(buf1, "%lld", nR2);
    status = add_tbl(t2, buf1, &t2_id, &t2_rec); cBYE(status);
  }
  // Get nn field for f1 if if it exists
  if ( nn_t1f1_id >= 0 ) { 
    status = get_data(nn_t1f1_rec, &nnX, &n_nnX, 0);
  }
  //--------------------------------------------------------
  /* Allocate space for output */
  int fldsz; size_t filesz;
  int ddir_id = -1, fileno = -1;
  int nn_ddir_id = -1, nn_fileno = -1;
  status = get_fld_sz(t1f1_rec.fldtype, &fldsz);
  filesz = nR2 * fldsz;
  status = mk_temp_file(filesz, &ddir_id, &fileno); cBYE(status);
  status = q_mmap(ddir_id, fileno, &opX, &n_opX, 1); cBYE(status);
  //--------------------------------------------------------
  /* Allocate space for output condition field if necessary */
  if ( nnX != NULL ) { 
    int nn_fldsz; size_t nn_filesz;
    status = get_fld_sz(I1, &nn_fldsz);
    nn_filesz = nR2 * nn_fldsz;
    status = mk_temp_file(nn_filesz, &nn_ddir_id, &nn_fileno); cBYE(status);
    status = q_mmap(nn_ddir_id, nn_fileno, &nnopX, &n_nnopX, true);cBYE(status);
  }
  //--------------------------------------------------------
  // Get started
  if ( cfldX == NULL ) { 
    /* Optimization when (i) no condition field or (ii) range selected */
    if ( is_cfld_a_range == true ) { 
      memcpy(opX, X+(lb*fldsz), ((ub-lb) * fldsz));
      if ( nnX != NULL ) { 
	memcpy(nnopX, nnX+(lb*sizeof(char)), ((ub-lb) * sizeof(char)));
	is_null_val_in_op = true;
      }
    }
    else {
      memcpy(opX, X, (nR1 * fldsz));
      if ( nnX != NULL ) { 
	memcpy(nnopX, nnX, nR1 * sizeof(char));
	is_null_val_in_op = true;
      }
    }
  }
  else {
    char *bak_X = X, *bak_nnX = nnX, *bak_opX = opX, *bak_nnopX = nnopX;
    long long dbg_nR2 = 0;
    char      *opI1 = (char      *)opX; char      *ipI1 = (char *)X;
    short     *opI2 = (short     *)opX; short     *ipI2 = (short *)X;
    int       *opI4 = (int       *)opX; int       *ipI4 = (int *)X;
    float     *opF4 = (float     *)opX; float     *ipF4 = (float *)X;
    double    *opF8 = (double    *)opX; double    *ipF8 = (double *)X;
    long long *opI8 = (long long *)opX; long long *ipI8 = (long long *)X;
    if ( nnX == NULL ) { 
      switch ( t1f1_rec.fldtype ) { 
	case I1 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI1[dbg_nR2++] = ipI1[i];
	    }
	  }
	  break;
	case I2 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI2[dbg_nR2++] = ipI2[i];
	    }
	  }
	  break;
	case I4 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI4[dbg_nR2++] = ipI4[i];
	    }
	  }
	  break;
	case I8 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opI8[dbg_nR2++] = ipI8[i];
	    }
	  }
	  break;
	case F4 :
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opF4[dbg_nR2++] = ipF4[i];
	    }
	  }
	  break;
	case F8 : 
	  for ( long long i = 0; i < nR1; i++ ) { 
	    if ( cfldX[i] == TRUE ) { 
	      opF8[dbg_nR2++] = ipF8[i];
	    }
	  }
	  break;
	default : go_BYE(-1); break;
      }

      /* OLD 
      for ( long long i = 0; i < nR1; i++ ) {
	if ( cfldX[i] == TRUE ) { 
	  memcpy(opX, X, fldsz);
	  opX += fldsz;
	  dbg_nR2++;
	}
	X += fldsz;
      }
      */
    }
    else {
      for ( long long i = 0; i < nR1; i++ ) {
	if ( cfldX[i] == TRUE ) { 
	  memcpy(opX, X, fldsz);
	  opX += fldsz;
	  *nnopX = *nnX;
	  if ( *nnopX == FALSE ) { is_null_val_in_op = true; }
	  nnopX++;
	  nnX++;
	  dbg_nR2++;
	}
	X += fldsz;
      }
    }
    if ( dbg_nR2 != nR2 ) { go_BYE(-1); } 
    X     = bak_X;
    nnX   = bak_nnX;
    opX   = bak_opX;
    nnopX = bak_nnopX;
  }

  // Add field to meta data store 
  zero_fld_rec(&t2f1_rec); t2f1_rec.fldtype = t1f1_rec.fldtype;
  status = add_fld(t2_id, f1, ddir_id, fileno, &t2f1_id, &t2f1_rec);
  cBYE(status);
  /*- Some other meta data to take care of --------------------*/
  /* Sort status stays the same */
  g_flds[t2f1_id].srttype     = g_flds[t1f1_id].srttype;
  // Transfer meta information from src_fld to dst_fld
  status = copy_meta(t2f1_id, t1f1_id, 1); cBYE(status);
  /*-----------------------------------------------------------*/
  // TODO P3: Note that because we are only copying some of the values,
  // it is possible for input field to have null values but output field
  // to be all defined. We have not taken care of this case. 
  if ( ( nnX != NULL ) && ( is_null_val_in_op ) ) {
    zero_fld_rec(&nn_t2f1_rec); nn_t2f1_rec.fldtype = I1;
    status = add_aux_fld(NULL, t2_id, NULL, t2f1_id, nn_ddir_id, nn_fileno, 
			 nn, &nn_t2f1_id, &nn_t2f1_rec);
    cBYE(status);
  }
  else {
    if ( nn_fileno >= 0 ) { /* if you did create an nn field */
      status = q_delete(nn_ddir_id, nn_fileno); cBYE(status);
    }
  }
 BYE:
  rs_munmap(X, nX);
  rs_munmap(nnX, n_nnX);
  rs_munmap(opX, n_opX);
  rs_munmap(nnopX, n_nnopX);
  rs_munmap(cfldX, n_cfldX);
  return status ;
}
