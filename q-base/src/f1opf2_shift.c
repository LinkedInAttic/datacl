#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "auxil.h"
#include "aux_meta.h"
#include "f1opf2_shift.h"
#include "assign_I1.h"
// last review 9/5/2013

//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2_shift(
	     char *X,
	     long long nR,
	     FLD_TYPE fldtype,
	     int n_s, /* amount to shift by */
	     char *str_newval,
	     char *op_X
	     )
// STOP FUNC DECL
{
  int status = 0;
  int fldsz;
  char      valI1, *f2I1 = NULL; 
  short     valI2, *f2I2 = NULL; 
  int       valI4, *f2I4 = NULL; 
  long long valI8, *f2I8 = NULL;
  float     valF4, *f2F4 = NULL; 
  double    valF8, *f2F8 = NULL;

  switch ( fldtype ) { 
  case I1 : 
    status = stoI1(str_newval, &valI1); cBYE(status); f2I1 = (char *)op_X;
    break;
  case I2 : 
    status = stoI2(str_newval, &valI2); cBYE(status); f2I2 = (short *)op_X;
    break;
  case I4 : 
    status = stoI4(str_newval, &valI4); cBYE(status); f2I4 = (int *)op_X;
    break;
  case I8 : 
    status = stoI8(str_newval, &valI8); cBYE(status); f2I8 = (long long *)op_X;
    break;
  case F4 : 
    status = stoF4(str_newval, &valF4); cBYE(status); f2F4 = (float *)op_X;
    break;
  case F8 : 
    status = stoF8(str_newval, &valF8); cBYE(status); f2F8 = (double *)op_X;
    break;
  default : go_BYE(-1); break;
  }

  if ( n_s == 0 ) { go_BYE(-1); }
  if ( ( n_s <= -MAX_SHIFT ) || (n_s >= MAX_SHIFT ) ) { go_BYE(-1); }
  if ( ( n_s <= -1 * nR ) || (n_s >= nR ) ) { go_BYE(-1); }

  status = get_fld_sz(fldtype, &fldsz); cBYE(status);
  if ( n_s > 0 ) {  /* shift down */
    memcpy(op_X + (fldsz*n_s), X, (nR-n_s)*fldsz);
    // set remaining values to newval
    switch ( fldtype ) { 
    case I1 : for ( int i = 0; i < n_s; i++ ) { f2I1[i] = valI1; } break;
    case I2 : for ( int i = 0; i < n_s; i++ ) { f2I2[i] = valI2; } break;
    case I4 : for ( int i = 0; i < n_s; i++ ) { f2I4[i] = valI4; } break;
    case I8 : for ( int i = 0; i < n_s; i++ ) { f2I8[i] = valI8; } break;
    case F4 : for ( int i = 0; i < n_s; i++ ) { f2F4[i] = valF4; } break;
    case F8 : for ( int i = 0; i < n_s; i++ ) { f2F8[i] = valF8; } break;
    default : go_BYE(-1); break;
    }
  }
  else if ( n_s < 0 ) {  /* shift up */
    n_s *= -1;
    memcpy(op_X, X + (fldsz*n_s), (nR-n_s)*fldsz);
    // set remaining values to newval
    switch ( fldtype ) { 
    case I1 : for ( int i = 0; i < n_s; i++ ) { f2I1[nR-1-i] = valI1; } break;
    case I2 : for ( int i = 0; i < n_s; i++ ) { f2I2[nR-1-i] = valI2; } break;
    case I4 : for ( int i = 0; i < n_s; i++ ) { f2I4[nR-1-i] = valI4; } break;
    case I8 : for ( int i = 0; i < n_s; i++ ) { f2I8[nR-1-i] = valI8; } break;
    case F4 : for ( int i = 0; i < n_s; i++ ) { f2F4[nR-1-i] = valF4; } break;
    case F8 : for ( int i = 0; i < n_s; i++ ) { f2F8[nR-1-i] = valF8; } break;
    default : go_BYE(-1); break;
    }
  }
  else { go_BYE(-1); }

 BYE:
  return status ;
}
