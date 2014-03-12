#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "auxil.h"
#include "aux_meta.h"
#include "extract_S.h"

// START FUNC DECL
int
get_type_op_fld(
		char *fn,
		char *op,
		FLD_TYPE f1_fldtype, 
		FLD_TYPE f2_fldtype, 
		char *aux_info,
		FLD_TYPE *ptr_op_fldtype
		)
// STOP FUNC DECL
{
  int status = 0;
  if ( strcmp(fn, "f1f2opf3") == 0 ) {
    if ( strcmp(op, "concat") == 0 ) {
      *ptr_op_fldtype = I8;
      return(status);
    }
  }
  if ( ( strcmp(fn, "f1f2opf3") == 0 ) || 
       ( strcmp(fn, "f1s1opf2") == 0 ) ) {
    if ( 
	( strcmp(op, "+") == 0 ) || 
	( strcmp(op, "-") == 0 ) || 
	( strcmp(op, "*") == 0 ) || 
	( strcmp(op, "/") == 0 ) || 
	( strcmp(op, "^") == 0 ) || 
	( strcmp(op, "|") == 0 ) || 
	( strcmp(op, "&") == 0 ) || 
	( strcmp(op, "%") == 0 ) ||
	( strcmp(op, "<<") == 0 ) ||
	( strcmp(op, ">>") == 0 ) ) {
      if ((f1_fldtype == I4 ) && ( f2_fldtype == I4)){
	*ptr_op_fldtype = I4;
      }
      else if ((f1_fldtype == I1 ) && ( f2_fldtype == I1)){
	*ptr_op_fldtype = I1;
      }
      else if ((f1_fldtype == I2 ) && ( f2_fldtype == I2)){
	*ptr_op_fldtype = I2;
      }
      else if ((f1_fldtype == F4 ) && ( f2_fldtype == F4)){
	*ptr_op_fldtype = F4;
      }
      else if ((f1_fldtype == I8 ) && ( f2_fldtype == I8)){
	*ptr_op_fldtype = I8;
      }
      else if ((f1_fldtype == F8 ) && ( f2_fldtype == F8)){
	*ptr_op_fldtype = F8;
      }
      else {
	go_BYE(-1);
      }
    }
    else if ( 
	     ( strcmp(op, "<=") == 0 ) || 
	     ( strcmp(op, ">=") == 0 ) || 
	     ( strcmp(op, "==") == 0 ) || 
	     ( strcmp(op, "!=") == 0 ) || 
	     ( strcmp(op, "<") == 0 ) || 
	     ( strcmp(op, ">") == 0 ) ||
	     ( strcmp(op, "<||>") == 0 ) ||
	     ( strcmp(op, "<=||>=") == 0 ) ||
	     ( strcmp(op, ">&&<") == 0 ) ||
	     ( strcmp(op, ">=&&<=") == 0 ) ) {
      *ptr_op_fldtype = I1;
      if ( f1_fldtype != f2_fldtype ) { go_BYE(-1); }
      switch ( f1_fldtype ) { 
      case I1 :
      case I2 :
      case I4 :
      case I8 :
      case F4 :
      case F8 :
	break;
      default : go_BYE(-1); break;
      }
    }
    else if ( 
	     ( strcmp(op, "&&") == 0 ) || 
	     ( strcmp(op, "&&!") == 0 ) || 
	     ( strcmp(op, "||") == 0 )  ) {
      if ( ( f1_fldtype == I1 ) && ( f2_fldtype == I1 ) ) {
        *ptr_op_fldtype = I1;
      }
      else if ( ( f1_fldtype == B ) && ( f2_fldtype == B ) ) {
        *ptr_op_fldtype = B;
      }
      else {
	fprintf(stderr, "Unknown op = %s \n", op);
	go_BYE(-1);
      }
    }
  }
  else if ( strcmp(fn, "f1opf2") == 0 ) {
    bool is_newtype_null; char newtype[16]; zero_string(newtype, 16);
    status = extract_S(aux_info, "newtype=[", "]", newtype, 16, 
	  &is_newtype_null);
    if ( ( strcmp(op, "conv") == 0 ) || ( strcmp(op, "cum") == 0 ) ) {
      if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
      if ( is_newtype_null ) { go_BYE(-1); }
      *ptr_op_fldtype = undef_fldtype;
      status = unstr_fldtype(newtype, ptr_op_fldtype); cBYE(status);
      if ( *ptr_op_fldtype == undef_fldtype ) { go_BYE(-1); }
      if ( strcmp(op, "conv") == 0 ) {
        if ( *ptr_op_fldtype == f1_fldtype ) { go_BYE(-1); }
      }
      else if ( strcmp(op, "cum") == 0 ) {
	switch ( *ptr_op_fldtype ) { 
	  case I1 : case I2 : case I4 : case I8 : break;
	  default : go_BYE(-1); break;
	}
      }
      else {
	go_BYE(-1);
      }
    }
    else {
      switch ( f1_fldtype ) {
      case I4 : 
	if ( 
	    ( strcmp(op, "++") == 0 ) ||
	    ( strcmp(op, "--") == 0 ) ||
	    ( strcmp(op, "!") == 0 ) ||
	    ( strcmp(op, "~") == 0 ) ||
	    ( strcmp(op, "shift") == 0 ) ) {
	  *ptr_op_fldtype = I4;
	}
	else if ( strcmp(op, "hash") == 0 ) {
	  *ptr_op_fldtype = I8;
	}
	else if ( strcmp(op, "bitcount") == 0 ) {
	  *ptr_op_fldtype = I1;
	}
	else {
	  go_BYE(-1);
	}
	break;
      case I8 : 
	if ( strcmp(op, "bitcount") == 0 ) {
	  *ptr_op_fldtype = I1;
	}
	else if ( 
		 ( strcmp(op, "++") == 0 ) ||
		 ( strcmp(op, "--") == 0 ) ||
		 ( strcmp(op, "!") == 0 ) ||
		 ( strcmp(op, "~") == 0 ) ||
		 ( strcmp(op, "shift") == 0 ) ||
		 ( strcmp(op, "hash") == 0 ) ) { 
	  *ptr_op_fldtype = I8;
	}
	else {
	  go_BYE(-1);
	}
	break;
      case I2 : 
	if ( strcmp(op, "shift") == 0 ) {
	  *ptr_op_fldtype = I2;
	}
	else {
	  go_BYE(-1);
	}
	break;
      case I1 : 
	if ( ( strcmp(op, "!") == 0 ) || 
	     ( strcmp(op, "~") == 0 ) || 
	     ( strcmp(op, "smear") == 0 ) ||
	     ( strcmp(op, "shift") == 0 ) ) {
	  *ptr_op_fldtype = I1;
	}
	else if ( strcmp(op, "idx_with_reset") == 0 ) {
	  *ptr_op_fldtype = I8;
	}
	else {
	  go_BYE(-1); 
	}
	break;
      case F4 : 
	if ( ( strcmp(op, "sqrt") == 0 ) || 
	     ( strcmp(op, "abs") == 0 ) || 
	     ( strcmp(op, "shift") == 0 ) ) { 
	  *ptr_op_fldtype = F4;
	}
	  else {
	    go_BYE(-1);
	  }
	  break;
	case F8 : 
	  if ( ( strcmp(op, "sqrt") == 0 ) || 
	       ( strcmp(op, "abs") == 0 ) || 
	       ( strcmp(op, "shift") == 0 ) ) { 
	    *ptr_op_fldtype = F8;
	  }
	  else if ( strcmp(op, "normal_cdf_inverse") == 0 ) {
	    *ptr_op_fldtype = F8;
	  }
	  else {
	    go_BYE(-1);
	  }
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  }
 BYE:
  return(status);
}
