/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#ifdef IPP
#include "ipps.h"
#endif
#include "qtypes.h"
#include "mmap.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "f1opf2_shift.h"
#include "assign_I1.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2_shift(
	     char *X,
	     char *nn_X,
	     long long nR,
	     FLD_TYPE fldtype,
	     int shift_by,
	     char *op_X,
	     char *nn_op_X
    )
// STOP FUNC DECL
{
  int status = 0;
  int fldsz;
  char zeros[1024*8];

  for ( int i = 0; i < 1024*8; i++ ) { zeros[i] = 0; }
  if ( ( shift_by <= -1024 ) || (shift_by >= 1024 ) ) { go_BYE(-1); }
  if ( ( shift_by <= -1 * nR ) || (shift_by >= nR ) ) { go_BYE(-1); }

  status = get_fld_sz(fldtype, &fldsz); cBYE(status);
  if ( shift_by > 0 ) {  /* shift down */
    memcpy(op_X + (fldsz*shift_by), X, (nR-shift_by)*fldsz);
    // set remaining values to 0
    memcpy(op_X, zeros, (shift_by *fldsz));
    // Deal with nn field 
    if  ( nn_X != NULL ) { 
      memcpy(nn_op_X + (nR-shift_by), zeros, shift_by);
    }
    else {
#ifdef IPP
      ippsSet_8u(1, nn_op_X+shift_by, (nR-shift_by));
#else
      assign_const_I1(nn_op_X+shift_by, (nR-shift_by), 1);
#endif
    }
    memcpy(nn_op_X, zeros, shift_by);
  }
  else if ( shift_by < 0 ) {  /* shift up */
    shift_by *= -1;
    memcpy(op_X, X + (fldsz*shift_by), (nR-shift_by)*fldsz);
    // set remaining values to 0
    memcpy(op_X + ((nR-shift_by)*fldsz), zeros, (shift_by *fldsz));
    // Deal with nn field 
    if  ( nn_X != NULL ) { 
      memcpy(op_X + ((nR-shift_by)*fldsz), zeros, (shift_by *fldsz));
    }
    else {
#ifdef IPP
      ippsSet_8u(1, nn_op_X, (nR-shift_by));
#else
      assign_const_I1(nn_op_X, (nR-shift_by), 1);
#endif
    }
    memcpy(nn_op_X + (nR-shift_by), zeros, shift_by);
  }
  else { go_BYE(-1); }

BYE:
  return(status);
}
