#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "auxil.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "aux_meta.h"
#include "assign_I1.h"
#include "assign_I2.h"
#include "assign_I4.h"
#include "assign_I8.h"

// last review 9/10/2013
//---------------------------------------------------------------
// START FUNC DECL
int 
set_val(
	char *tbl,
	char *fld,
	char *str_range,
	char *str_val
	)
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE tbl_rec;
  FLD_REC_TYPE fld_rec, nn_fld_rec;
  char *X = NULL; size_t nX = 0;
  char *nn_X = NULL; size_t nn_nX = 0; 
  char **Y = NULL; int nY = 0;
  char      *I1ptr = NULL, I1val = 0;
  short     *I2ptr = NULL, I2val = 0;
  int       *I4ptr = NULL, I4val = 0;
  long long *I8ptr = NULL, I8val = 0;

  int tbl_id, fld_id, nn_fld_id;
  long long lb, ub, nR;
#define BUFLEN 32
  char buf[BUFLEN];
  //----------------------------------------------------------------
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( fld == NULL ) || ( *fld == '\0' ) ) { go_BYE(-1); }
  if ( ( str_range == NULL ) || ( *str_range == '\0' ) ) { go_BYE(-1); }
  if ( ( str_val == NULL ) || ( *str_val == '\0' ) ) { go_BYE(-1); }
  //--------------------------------------------------------
  status = is_tbl(tbl, &tbl_id, &tbl_rec); cBYE(status);
  if ( tbl_id < 0 ) { go_BYE(-1); }
  nR = tbl_rec.nR;
  //--------------------------------------------------------
  status = is_fld(NULL, tbl_id, fld, &fld_id, &fld_rec, &nn_fld_id, &nn_fld_rec); cBYE(status);
  if ( fld_id < 0 ) { 
    fprintf(stderr, "Field [%s] not in Table [%s] \n", fld, tbl);
    go_BYE(-1);
  }
  if ( nn_fld_id >= 0 ) { 
    status = get_data(nn_fld_rec, &nn_X, &nn_nX, true); cBYE(status);
  }
  status = get_data(fld_rec, &X, &nX, true); cBYE(status);
  //--------------------------------------------------------
  zero_string(buf, BUFLEN);
  status = read_nth_val(str_range, ":", 0, buf, BUFLEN); cBYE(status);
  status = stoI8(buf, &lb); cBYE(status);
  if ( ( lb < 0 ) || ( lb >= nR ) ) { go_BYE(-1); }

  zero_string(buf, BUFLEN);
  status = read_nth_val(str_range, ":", 1, buf, BUFLEN); cBYE(status);
  status = stoI8(buf, &ub); cBYE(status);
  if ( ( ub < 0 ) || ( ub >= nR ) ) { go_BYE(-1); }

  if ( lb >= ub ) { go_BYE(-1); }
  if ( ( ub - lb ) >= INT_MAX ) { go_BYE(-1); } // because of IPP

  switch ( fld_rec.fldtype ) { 
  case I1 : status = stoI1(str_val, &I1val); cBYE(status); break;
  case I2 : status = stoI2(str_val, &I2val); cBYE(status); break;
  case I4 : status = stoI4(str_val, &I4val); cBYE(status); break;
  case I8 : status = stoI8(str_val, &I8val); cBYE(status); break;
  default : go_BYE(-1); break;
  }
  switch ( fld_rec.fldtype ) { 
  case I1 :
    I1ptr = (char *)X; I1ptr += lb;
#ifdef IPP
    if ( ( I1val >= 0 ) && ( I1val <= SCHAR_MAX ) )  {
      ippsSet_8u(I1val, I1ptr, nX);
    }
    else {
      assign_const_I1(I1ptr, nX, I1val); 
    }
#else
    assign_const_I1(I1ptr, nX, I1val); 
#endif
    break;
#include "incl_set_val_I2.c"
#include "incl_set_val_I4.c"
#include "incl_set_val_I8.c"
  default : go_BYE(-1); break;
  }
  if ( nn_X != NULL ) { 
#ifdef IPP
    ippsSet_8u(1, nn_X+lb, nX);
#else
    assign_const_I1(nn_X + lb, nX, 1);
#endif
  }
  // P2 TODO: Need to make sure that if nn field is no longer needed, then
  // we delete the field 
 BYE:
  if ( Y != NULL ) { 
    for ( int i = 0; i < nY; i++ ) {
      free_if_non_null(Y[i]);
    }
    free_if_non_null(Y);
  }
  rs_munmap(X, nX);
  rs_munmap(nn_X, nn_nX);
  return(status);
}
