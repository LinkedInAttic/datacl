#include "qtypes.h"
#include "auxil.h"
#include "get_meta.h"

#include "is_s_in_f_seq_I1.h"
#include "is_s_in_f_seq_I2.h"
#include "is_s_in_f_seq_I4.h"
#include "is_s_in_f_seq_I8.h"

#include "bin_search_I1.h"
#include "bin_search_I2.h"
#include "bin_search_I4.h"
#include "bin_search_I8.h"

// last review 9/12/2013
// START FUNC DECL
int
is_s_in_f(
    char *X,
    char *nn_X,
    FLD_TYPE fldtype,
    char *str_srttype,
    char *str_val,
    long long nR,
    long long *ptr_idx
    )
// STOP FUNC DECL
{
  int status = 0;
  long long idx = -1;
  char      I1val, *I1ptr = (char      *)X;
  short     I2val, *I2ptr = (short     *)X;
  int       I4val, *I4ptr = (int       *)X;
  long long I8val, *I8ptr = (long long *)X;

  *ptr_idx = -1;
  if ( X    == NULL ) { go_BYE(-1); }
  if ( str_val == NULL ) { go_BYE(-1); }
  if ( str_srttype == NULL ) { go_BYE(-1); }

  switch ( fldtype ) { 
    case I1 : status = stoI1(str_val, &I1val); cBYE(status); break;
    case I2 : status = stoI2(str_val, &I2val); cBYE(status); break;
    case I4 : status = stoI4(str_val, &I4val); cBYE(status); break;
    case I8 : status = stoI8(str_val, &I8val); cBYE(status); break;
    default : go_BYE(-1); break;
  }

  if ( strcmp(str_srttype, "ascending") == 0 ) { /* binary search */
    switch ( fldtype ) { 
      case I1 : 
	status = bin_search_I1(I1ptr, nR, I1val, &idx, ""); cBYE(status);
	break;
      case I2 : 
	status = bin_search_I2(I2ptr, nR, I2val, &idx, ""); cBYE(status);
	break;
      case I4 : 
	status = bin_search_I4(I4ptr, nR, I4val, &idx, ""); cBYE(status);
	break;
      case I8 : 
	status = bin_search_I8(I8ptr, nR, I8val, &idx, ""); cBYE(status);
	break;
      default : 
	go_BYE(-1);
	break;
    }
  }
  else { /* sequential search */
    switch ( fldtype ) { 
      case I1 : 
        is_s_in_f_seq_I1(I1ptr, nn_X, I1val, nR, &idx);cBYE(status);
	break;
      case I2 : 
        is_s_in_f_seq_I2(I2ptr, nn_X, I2val, nR, &idx);cBYE(status);
	break;
      case I4 : 
        is_s_in_f_seq_I4(I4ptr, nn_X, I4val, nR, &idx);cBYE(status);
	break;
      case I8 : 
        is_s_in_f_seq_I8(I8ptr, nn_X, I8val, nR, &idx);cBYE(status);
	break;
      default : 
	go_BYE(-1);
	break;
    }
  }
  *ptr_idx = idx;
BYE:
  return status ;
}
