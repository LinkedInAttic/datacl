#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "get_sz_type_op_fld.h"
#include "fsize.h"
#include "mk_file.h"

#include "cum_int_to_int.h"
#include "cum_int_to_longlong.h"
#include "cum_longlong_to_int.h"
#include "cum_longlong_to_longlong.h"
#include "cum_bool_to_int.h"
#include "cum_bool_to_longlong.h"

#include "cum_reset_int_to_int.h"
#include "cum_reset_int_to_longlong.h"
#include "cum_reset_longlong_to_int.h"
#include "cum_reset_longlong_to_longlong.h"
#include "cum_reset_bool_to_int.h"
#include "cum_reset_bool_to_longlong.h"
// START FUNC DECL
int 
f1opf2_cum(
	   char *in_X,
	   long long nR,
	   int f1type, /* input type */
	   int f2type, /* output type */
	   bool is_reset,
	   long long reset_on,
	   long long reset_to,
	   char **ptr_opfile
	   )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ofp = NULL; char *opfile = NULL;
  int sizeof_op;
  char *op_X = NULL; size_t n_op_X = 0;


  // TODO: Is it okay to have nR == 1 ? if ( nR <= 1 ) { go_BYE(-1); }
  // create an output file and mmap it 
  status = open_temp_file(&ofp, &opfile, 0);
  cBYE(status);
  fclose_if_non_null(ofp);
  switch ( f2type ) { 
  case FLDTYPE_INT      : sizeof_op = sizeof(int); break; 
  case FLDTYPE_LONGLONG : sizeof_op = sizeof(long long); break; 
  default : go_BYE(1); break;
  }
  //------------------------------------------
  n_op_X = sizeof_op * nR;
  status = mk_file(opfile, n_op_X);
  cBYE(status);
  status = rs_mmap(opfile, &op_X, &n_op_X, 1);
  cBYE(status);
  //--------------------------------------------
  switch ( f1type ) { 
  case FLDTYPE_INT : 
    switch ( f2type ) { 
    case FLDTYPE_INT: 
      if ( !is_reset ) { 
        cum_int_to_int((int *)in_X, nR, (int *)op_X);
      }
      else {
        cum_reset_int_to_int((int *)in_X, nR, (int)reset_on,
	    (int)reset_to, (int *)op_X);
      }
      break;
    case FLDTYPE_LONGLONG: 
      if ( !is_reset ) {
	cum_int_to_longlong((int *)in_X, nR, (long long *)op_X);
      }
      else { go_BYE(-1); } // TODO

      break;
    default : 
      go_BYE(-1);
      break;
    }
    break;
  case FLDTYPE_LONGLONG : 
    switch ( f2type ) { 
    case FLDTYPE_INT: 
      if ( !is_reset ) { 
	cum_longlong_to_int((long long *)in_X, nR, (int *)op_X);
      }
      else { go_BYE(-1); } // TODO
      break;
    case FLDTYPE_LONGLONG: 
      if ( !is_reset ) { 
	cum_longlong_to_longlong((long long *)in_X, nR, (long long *)op_X);
      }
      else { go_BYE(-1); } // TODO
      break;
    default : 
      go_BYE(-1);
      break;
    }
    break;
  case FLDTYPE_BOOL : 
    switch ( f2type ) { 
    case FLDTYPE_INT: 
      if ( !is_reset ) { 
	cum_bool_to_int((char *)in_X, nR, (int *)op_X);
      }
      else { go_BYE(-1); } // TODO
      break;
    case FLDTYPE_LONGLONG: 
      if ( !is_reset ) { 
	cum_bool_to_longlong((char *)in_X, nR, (long long *)op_X);
      }
      else { go_BYE(-1); } // TODO
      break;
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
  *ptr_opfile = opfile;
 BYE:
  return(status);
}
