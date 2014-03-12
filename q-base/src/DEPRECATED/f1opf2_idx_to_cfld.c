#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "mmap.h"
#include "mk_temp_file.h"
#include "aux_meta.h"
#include "f1opf2_cum.h"

#include "cum_int_to_int.h"
#include "cum_int_to_longlong.h"
#include "cum_longlong_to_int.h"
#include "cum_longlong_to_longlong.h"
#include "cum_bool_to_int.h"
#include "cum_bool_to_longlong.h"

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
        cum_int_to_int((int *)in_X, nR, (int *)op_X);
      break;
    case I8: 
	cum_int_to_longlong((int *)in_X, nR, (long long *)op_X);
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
	cum_longlong_to_int((long long *)in_X, nR, (int *)op_X);
    case I8: 
	cum_longlong_to_longlong((long long *)in_X, nR, (long long *)op_X);
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
	cum_bool_to_int((char *)in_X, nR, (int *)op_X);
      break;
    case I8: 
	cum_bool_to_longlong((char *)in_X, nR, (long long *)op_X);
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
  return status ;
}
