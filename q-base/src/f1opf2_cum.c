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
#include "cum_I2_to_I4.h"
#include "cum_I2_to_I8.h"

// START FUNC DECL
int 
f1opf2_cum(
	   const char *in_X,
	   long long nR,
	   FLD_TYPE f1type, /* input type */
	   FLD_TYPE f2type, /* output type */
	   char * restrict out_X
	   )
// STOP FUNC DECL
{
  int status = 0;
  char       *f1I1 = NULL;
  short      *f1I2 = NULL;
  int        *f1I4 = NULL, *f2I4 = NULL;
  long long  *f1I8 = NULL, *f2I8 = NULL;

  switch ( f1type ) { 
  case I1 : f1I1 = (char      *)in_X; break; 
  case I2 : f1I2 = (short     *)in_X; break; 
  case I4 : f1I4 = (int       *)in_X; break; 
  case I8 : f1I8 = (long long *)in_X; break; 
  default : go_BYE(-1); break;
  }
  switch ( f2type ) { 
  case I4 : f2I4 = (int       *)out_X; break; 
  case I8 : f2I8 = (long long *)out_X; break; 
  default : go_BYE(-1); break;
  }
  // It is okay to have nR == 1
  switch ( f1type ) {
  case I1 : 
    switch ( f2type ) { 
    case I4: cum_I1_to_I4(f1I1, nR, f2I4); break;
    case I8: cum_I1_to_I8(f1I1, nR, f2I8); break;
    default : go_BYE(-1); break;
    }
    break;
  case I2 : 
    switch ( f2type ) { 
    case I4: cum_I2_to_I4(f1I2, nR, f2I4); break;
    case I8: cum_I2_to_I8(f1I2, nR, f2I8); break;
    default : go_BYE(-1); break;
    }
    break;
  case I4 : 
    switch ( f2type ) { 
    case I4: cum_I4_to_I4(f1I4, nR, f2I4); break;
    case I8: cum_I4_to_I8(f1I4, nR, f2I8); break;
    default : go_BYE(-1); break;
    }
    break;
  case I8 : 
    switch ( f2type ) { 
    case I4: cum_I8_to_I4(f1I8, nR, f2I4); break; 
    case I8: cum_I8_to_I8(f1I8, nR, f2I8); break;
    default : go_BYE(-1); break;
    }
    break;
  default : 
    go_BYE(-1);
    break;
  }
 BYE:
  return status ;
}
