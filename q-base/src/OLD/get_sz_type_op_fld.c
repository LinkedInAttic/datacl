#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"

// START FUNC DECL
int
get_sz_type_op_fld(
		   char *fn,
		   char *op,
		   int f1type,
		   int f2type,
		   char *aux_info,
		   int *ptr_op_type,
		   int *ptr_op_sz
		   )
// STOP FUNC DECL
{
  int status = 0;
  if ( strcmp(fn, "f1f2opf3") == 0 ) {
    if ( strcmp(op, "concat") == 0 ) {
      *ptr_op_type = FLDTYPE_LONGLONG;
      *ptr_op_sz = sizeof(long long);
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
      if ((f1type == FLDTYPE_INT ) && ( f2type == FLDTYPE_INT)){
	*ptr_op_type = FLDTYPE_INT;
	*ptr_op_sz = sizeof(int);
      }
      else if ((f1type == FLDTYPE_FLOAT ) && ( f2type == FLDTYPE_FLOAT)){
	*ptr_op_type = FLDTYPE_FLOAT;
	*ptr_op_sz = sizeof(float);
      }
      else if ((f1type == FLDTYPE_LONGLONG ) && ( f2type == FLDTYPE_LONGLONG)){
	*ptr_op_type = FLDTYPE_LONGLONG;
	*ptr_op_sz = sizeof(long long);
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
	     ( strcmp(op, ">") == 0 ) ) {
      *ptr_op_type = FLDTYPE_BOOL;
      if ( ( f1type == FLDTYPE_INT ) && ( f2type == FLDTYPE_INT ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else if ( ( f1type == FLDTYPE_FLOAT ) && ( f2type == FLDTYPE_FLOAT ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else if ( ( f1type == FLDTYPE_LONGLONG ) && ( f2type == FLDTYPE_LONGLONG ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else if ( ( f1type == FLDTYPE_BOOL ) && ( f2type == FLDTYPE_BOOL ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else if ( ( f1type == FLDTYPE_CHAR_STRING ) && ( f2type == FLDTYPE_CHAR_STRING ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else {
	go_BYE(1);
      }
    }
    else if ( 
	     ( strcmp(op, "&&") == 0 ) || 
	     ( strcmp(op, "||") == 0 )  ) {
      *ptr_op_type = FLDTYPE_BOOL;
      if ( ( f1type == FLDTYPE_BOOL ) && ( f2type == FLDTYPE_BOOL ) ) {
	*ptr_op_sz = sizeof(char);
      }
      else {
	go_BYE(-1);
      }
    }
  }
  else if ( strcmp(fn, "f1opf2") == 0 ) {
    if ( f1type == FLDTYPE_CHAR_STRING ) {
      if ( strcmp(op, "dateconv") == 0 ) {
	*ptr_op_sz = sizeof(int);
	*ptr_op_type = FLDTYPE_INT;
      }
      else { go_BYE(-1); }
    }
    else if ( f1type == FLDTYPE_DOUBLE ) {
      if ( strcmp(op, "conv") == 0 ) {
	if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
	if ( strcmp(aux_info, "int") == 0 ) {
	  *ptr_op_sz = sizeof(int);
	  *ptr_op_type = FLDTYPE_INT;
	}
	else if ( strcmp(aux_info, "long long") == 0 ) {
	  *ptr_op_sz = sizeof(long long);
	  *ptr_op_type = FLDTYPE_LONGLONG;
	}
	else if ( strcmp(aux_info, "float") == 0 ) {
	  *ptr_op_sz = sizeof(float);
	  *ptr_op_type = FLDTYPE_FLOAT;
	}
	else {
	  go_BYE(-1);
	}
      }
    }
    else if ( f1type == FLDTYPE_INT ) {
      if ( 
	  ( strcmp(op, "++") == 0 ) ||
	  ( strcmp(op, "--") == 0 ) ||
	  ( strcmp(op, "!") == 0 ) ||
	  ( strcmp(op, "~") == 0 ) ||
	  ( strcmp(op, "bitcount") == 0 ) ||
	  ( strcmp(op, "cum") == 0 ) ) {
	*ptr_op_sz = sizeof(int);
	*ptr_op_type = FLDTYPE_INT;
      }
      else if ( strcmp(op, "conv") == 0 ) {
	if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
	if ( strcmp(aux_info, "long long") == 0 ) {
	  *ptr_op_sz = sizeof(long long);
	  *ptr_op_type = FLDTYPE_LONGLONG;
	}
	else if ( strcmp(aux_info, "float") == 0 ) {
	  *ptr_op_sz = sizeof(float);
	  *ptr_op_type = FLDTYPE_FLOAT;
	}
	else if ( strcmp(aux_info, "bool") == 0 ) {
	  *ptr_op_sz = sizeof(char);
	  *ptr_op_type = FLDTYPE_BOOL;
	}
	else {
	  go_BYE(-1);
	}
      }
      else {
	go_BYE(-1);
      }
    }
    else if ( f1type == FLDTYPE_LONGLONG ) {
      if ( strcmp(op, "bitcount") == 0 ) {
	*ptr_op_sz = sizeof(int);
	*ptr_op_type = FLDTYPE_INT;
      }
      else if ( 
	       ( strcmp(op, "++") == 0 ) ||
	       ( strcmp(op, "--") == 0 ) ||
	       ( strcmp(op, "!") == 0 ) ||
	       ( strcmp(op, "~") == 0 ) ||
	       ( strcmp(op, "cum") == 0 ) ) {
	*ptr_op_sz = sizeof(long long);
	*ptr_op_type = FLDTYPE_LONGLONG;
      }
      else if ( strcmp(op, "conv") == 0 ) {
	if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
	if ( strcmp(aux_info, "int") == 0 ) {
	  *ptr_op_sz = sizeof(int);
	  *ptr_op_type = FLDTYPE_INT;
	}
	else if ( strcmp(aux_info, "float") == 0 ) {
	  *ptr_op_sz = sizeof(float);
	  *ptr_op_type = FLDTYPE_FLOAT;
	}
	else if ( strcmp(aux_info, "bool") == 0 ) {
	  *ptr_op_sz = sizeof(char);
	  *ptr_op_type = FLDTYPE_BOOL;
	}
	else {
	  go_BYE(-1);
	}
      }
      else {
	go_BYE(-1);
      }
    }
    else if ( f1type == FLDTYPE_BOOL ) {
      if ( strcmp(op, "conv") == 0 ) {
	if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
	if ( strcmp(aux_info, "int") == 0 ) {
	  *ptr_op_sz = sizeof(int);
	  *ptr_op_type = FLDTYPE_INT;
	}
	else if ( strcmp(aux_info, "long long") == 0 ) {
	  *ptr_op_sz = sizeof(long long);
	  *ptr_op_type = FLDTYPE_LONGLONG;
	}
	else if ( strcmp(aux_info, "float") == 0 ) {
	  *ptr_op_sz = sizeof(float);
	  *ptr_op_type = FLDTYPE_FLOAT;
	}
	else {
	  go_BYE(-1);
	}
      }
      else if ( strcmp(op, "!") == 0 ) {
	*ptr_op_sz = sizeof(char);
	*ptr_op_type = FLDTYPE_BOOL;
      }
      else if ( 
	       ( strcmp(op, "!") == 0 ) ||
	       ( strcmp(op, "~") == 0 ) ||
	       ( strcmp(op, "cum") == 0 ) ) {
	*ptr_op_sz = sizeof(int);
	*ptr_op_type = FLDTYPE_INT;
      }
    }
    else if ( f1type == FLDTYPE_FLOAT ) {
      if ( strcmp(op, "conv") == 0 ) {
	if ( ( aux_info == NULL ) || ( *aux_info == '\0' ) ) { go_BYE(-1); }
	if ( strcmp(aux_info, "int") == 0 ) {
	  *ptr_op_sz = sizeof(int);
	  *ptr_op_type = FLDTYPE_INT;
	}
	else if ( strcmp(aux_info, "long long") == 0 ) {
	  *ptr_op_sz = sizeof(long long);
	  *ptr_op_type = FLDTYPE_LONGLONG;
	}
	else if ( strcmp(aux_info, "bool") == 0 ) {
	  *ptr_op_sz = sizeof(char);
	  *ptr_op_type = FLDTYPE_BOOL;
	}
	else if ( strcmp(aux_info, "double") == 0 ) {
	  *ptr_op_sz = sizeof(double);
	  *ptr_op_type = FLDTYPE_DOUBLE;
	}
	else {
	  go_BYE(-1);
	}
      }
      else if ( strcmp(op, "sqrt") == 0 ) {
        *ptr_op_sz = sizeof(float);
        *ptr_op_type = FLDTYPE_FLOAT;
      }
      else {
	go_BYE(-1);
      }
    }
    else { 
      go_BYE(-1); 
    }
  }

  else { 
    go_BYE(-1);
  }
 BYE:
  return(status);
}
