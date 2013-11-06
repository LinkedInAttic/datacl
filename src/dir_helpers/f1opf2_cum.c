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
#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mmap.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "f1opf2_cum.h"

#include "cum_I4_to_I4.h"
#include "cum_I4_to_I8.h"
#include "cum_I8_to_I4.h"
#include "cum_I8_to_I8.h"
#include "cum_I1_to_I4.h"
#include "cum_I1_to_I8.h"

// START FUNC DECL
int 
f1opf2_cum(
	   const char *in_X,
	   long long nR,
	   FLD_TYPE f1type, /* input type */
	   FLD_TYPE f2type, /* output type */
	   int *ptr_ddir_id, 
	   char opfile[MAX_LEN_FILE_NAME+1]
	   )
// STOP FUNC DECL
{
  int status = 0;
  int fldsz = INT_MAX;
  char *op_X = NULL; size_t n_op_X = 0;

  // TODO: P2 Is it okay to have nR == 1 ? if ( nR <= 1 ) { go_BYE(-1); }
  // create an output file and mmap it 
  //------------------------------------------
  *ptr_ddir_id = INT_MAX;
  status = get_fld_sz(f2type, &fldsz);
  status = mk_temp_file(opfile, (fldsz * nR), ptr_ddir_id); cBYE(status);
  status = q_mmap(*ptr_ddir_id, opfile, &op_X, &n_op_X, 1); cBYE(status);
  //--------------------------------------------
  switch ( f1type ) { 
  case I4 : 
    switch ( f2type ) { 
    case I4: 
        cum_I4_to_I4((int *)in_X, nR, (int *)op_X);
      break;
    case I8: 
	cum_I4_to_I8((int *)in_X, nR, (long long *)op_X);
      break;
    case I1 : 
    case I2 : 
    case F4 :
    case F8 :
    default : 
      go_BYE(-1);
      break;
    }
    break;
  case I8 : 
    switch ( f2type ) { 
    case I4: 
	cum_I8_to_I4((long long *)in_X, nR, (int *)op_X);
    case I8: 
	cum_I8_to_I8((long long *)in_X, nR, (long long *)op_X);
      break;
    case I1 : 
    case I2 : 
    case F4 :
    case F8 :
    default : 
      go_BYE(-1);
      break;
    }
    break;
  case I1 : 
    switch ( f2type ) { 
    case I4: 
	cum_I1_to_I4((char *)in_X, nR, (int *)op_X);
      break;
    case I8: 
	cum_I1_to_I8((char *)in_X, nR, (long long *)op_X);
      break;
    case I1 : 
    case I2 : 
    case F4 :
    case F8 :
    default : 
      go_BYE(-1);
      break;
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
  rs_munmap(op_X, n_op_X);
 BYE:
  return(status);
}
