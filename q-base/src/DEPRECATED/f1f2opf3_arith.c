#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "f1f2opf3_arith.h"

//---------------------------------------------------------------
// START FUNC DECL
int 
f1f2opf3_arith(
	       long long nR,
	       FLD_META_TYPE f1_meta,
	       FLD_META_TYPE f2_meta,
	       char *str_meta_data,
	       char *f1_X,
	       char *nn_f1_X,
	       bool is_f2_scalar,
	       char *f2_X,
	       char *nn_f2_X,
	       char *op,
	       char **ptr_opfile,
	       char **ptr_nn_opfile,
	       bool *ptr_is_any_null
	       )
// STOP FUNC DECL
{
  int status = 0;
  int iop;
  FILE *ofp = NULL; char *opfile = NULL;
  FILE *nn_ofp = NULL; char *nn_opfile = NULL;

  status = open_temp_file(&ofp, &opfile);
  cBYE(status);
  status = open_temp_file(&nn_ofp, &nn_opfile);
  cBYE(status);
  sprintf(str_meta_data, "filename=%s", opfile);
  //--------------------------------------------
  /* works only for constant length fields */
  if ( f1_meta.n_sizeof == 0 ) { go_BYE(-1); }
  /* types must be same */
  if ( !is_f2_scalar ) { 
  if ( strcmp(f1_meta.fldtype, f2_meta.fldtype) != 0 ) {
    go_BYE(-1);
  }
  }
  //--- Set meta data for output field 
  status = mk_iop(op, &iop);
  cBYE(status);
  switch ( iop ) {
  case IOP_ADD : case IOP_SUB : case IOP_MUL : case IOP_DIV : 
    if ( strcmp(f1_meta.fldtype, "int") == 0 ) {
      strcat(str_meta_data, ":fldtype=int:n_sizeof=4");
      status = internal_f1f2opf3_arith_int(
	  (int *)f1_X, nn_f1_X, is_f2_scalar, (int *)f2_X, nn_f2_X, nR, 
	  iop, ofp, nn_ofp, ptr_is_any_null);
      cBYE(status);
    }
    else if ( strcmp(f1_meta.fldtype, "long long") == 0 ) {
      strcat(str_meta_data, ":fldtype=long long:n_sizeof=8");
      status = internal_f1f2opf3_arith_long_long(
	  (long long *)f1_X, nn_f1_X, is_f2_scalar, 
	  (long long *)f2_X, nn_f2_X, nR, 
	  iop, ofp, nn_ofp, ptr_is_any_null);
      cBYE(status);
    }
    else { go_BYE(-1); }
    break;
  case IOP_GT  : case IOP_LT  : case IOP_GEQ : 
  case IOP_LEQ : case IOP_NEQ : case IOP_EQ  : 
    strcat(str_meta_data, ":fldtype=char:n_sizeof=1");
    if ( strcmp(f1_meta.fldtype, "int") == 0 ) {
      status = internal_f1f2opf3_cond_int(
	  (int *)f1_X, nn_f1_X, is_f2_scalar, (int *)f2_X, nn_f2_X, nR, 
	  iop, ofp, nn_ofp, ptr_is_any_null);
      cBYE(status);
    }
    else if ( strcmp(f1_meta.fldtype, "long long") == 0 ) {
      status = internal_f1f2opf3_cond_longlong(
	  (long long *)f1_X, nn_f1_X, is_f2_scalar, (long long *)f2_X, 
	  nn_f2_X, nR, iop, ofp, nn_ofp, ptr_is_any_null);
      cBYE(status);
    }
    else { go_BYE(-1); }
    break;
  case BOP_AND : case BOP_OR : 
    if ( strcmp(f1_meta.fldtype, "char") == 0 ) {
      strcat(str_meta_data, ":fldtype=char:n_sizeof=1");
      status = internal_f1f2opf3_cond_bool(
	  f1_X, nn_f1_X, f2_X, nn_f2_X, nR, iop, ofp, nn_ofp, ptr_is_any_null);
      cBYE(status);
    }
    else { go_BYE(-1); }
    break;
  default: go_BYE(-1); break;
  }
  *ptr_opfile = opfile;
  *ptr_nn_opfile = nn_opfile;
 BYE:
  fclose_if_non_null(ofp);
  fclose_if_non_null(nn_ofp);
  return(status);
}
//-----------------------------------------------------------
// START FUNC DECL
int
internal_f1f2opf3_arith_int( 
			    int *i1ptr, 
			    char *nn_i1ptr,
			    bool is_f2_scalar,
			    int *i2ptr, 
			    char *nn_i2ptr,
			    long long nR, 
			    int iop,
			    FILE *ofp,
			    FILE *nn_ofp,
			    bool *ptr_is_any_null
			    )
// STOP FUNC DECL
{
  int status = 0;
  int ival; char nn_val;

  *ptr_is_any_null = false;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( ( nn_i1ptr == NULL ) || ( nn_i1ptr[i] == TRUE ) ) &&
         ( ( nn_i2ptr == NULL ) || ( nn_i2ptr[i] == TRUE ) ) ) {
      switch ( iop ) {
      case IOP_ADD : 
	if ( is_f2_scalar ) {
	  ival = i1ptr[i] + *i2ptr;
	}
	else {
  	  ival = i1ptr[i] + i2ptr[i];
	}
	break;
      case IOP_SUB : 
	if ( is_f2_scalar ) {
	  ival = i1ptr[i] - *i2ptr;
	}
	else {
	  ival = i1ptr[i] - i2ptr[i];
	}
	break;
      case IOP_MUL : 
	if ( is_f2_scalar ) {
	  ival = i1ptr[i] * *i2ptr;
	}
	else {
	  ival = i1ptr[i] * i2ptr[i];
	}
	break;
      case IOP_DIV : 
	if ( is_f2_scalar ) {
	  ival = i1ptr[i] / *i2ptr;
	}
	else {
	  ival = i1ptr[i] / i2ptr[i];
	}
	break;
      default:
	go_BYE(-1);
	break;
      }
      nn_val = TRUE;
    }
    else {
      ival = 0;
      nn_val = FALSE;
      *ptr_is_any_null = true;
    }
    fwrite(&ival, 1, sizeof(int), ofp);
    fwrite(&nn_val, 1, sizeof(char), nn_ofp);
  }
 BYE:
  return(status);
}
//-----------------------------------------------------------
// START FUNC DECL
int
internal_f1f2opf3_arith_long_long( 
			    long long *ll1ptr, 
			    char *nn_ll1ptr,
			    bool is_f2_scalar,
			    long long *ll2ptr, 
			    char *nn_ll2ptr,
			    long long nR, 
			    int iop,
			    FILE *ofp,
			    FILE *nn_ofp,
			    bool *ptr_is_any_null
			    )
// STOP FUNC DECL
{
  int status = 0;
  long long llval; char nn_val;

  *ptr_is_any_null = false;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( ( nn_ll1ptr == NULL ) || ( nn_ll1ptr[i] == TRUE ) ) &&
         ( ( nn_ll2ptr == NULL ) || ( nn_ll2ptr[i] == TRUE ) ) ) {
      switch ( iop ) {
      case IOP_ADD : 
	if ( is_f2_scalar ) {
	  llval = ll1ptr[i] + *ll2ptr;
	}
	else {
  	  llval = ll1ptr[i] + ll2ptr[i];
	}
	break;
      case IOP_SUB : 
	if ( is_f2_scalar ) {
	  llval = ll1ptr[i] - *ll2ptr;
	}
	else {
	  llval = ll1ptr[i] - ll2ptr[i];
	}
	break;
      case IOP_MUL : 
	if ( is_f2_scalar ) {
	  llval = ll1ptr[i] * *ll2ptr;
	}
	else {
	  llval = ll1ptr[i] * ll2ptr[i];
	}
	break;
      case IOP_DIV : 
	if ( is_f2_scalar ) {
	  llval = ll1ptr[i] / *ll2ptr;
	}
	else {
	  llval = ll1ptr[i] / ll2ptr[i];
	}
	break;
      default:
	go_BYE(-1);
	break;
      }
      nn_val = TRUE;
    }
    else {
      llval = 0;
      nn_val = FALSE;
      *ptr_is_any_null = true;
    }
    fwrite(&llval, 1, sizeof(long long), ofp);
    fwrite(&nn_val, 1, sizeof(char), nn_ofp);
  }
 BYE:
  return(status);
}
//-----------------------------------------------------------
// START FUNC DECL
int
internal_f1f2opf3_cond_int( 
			   int *i1ptr, 
			   char *nn_i1ptr,
			   bool is_f2_scalar,
			   int *i2ptr, 
			   char *nn_i2ptr,
			   long long nR, 
			   int iop,
			   FILE *ofp,
			   FILE *nn_ofp,
			   bool *ptr_is_any_null
			   )
// STOP FUNC DECL
{
  int status = 0;
  char cval, nn_val;

  *ptr_is_any_null = false;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( ( nn_i1ptr == NULL ) || ( nn_i1ptr[i] == TRUE ) ) &&
         ( ( nn_i2ptr == NULL ) || ( nn_i2ptr[i] == TRUE ) ) ) {
      switch ( iop ) {
      case IOP_GT : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] > *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] > i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_LT : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] < *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] < i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_GEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] >= *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] >= i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_LEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] <= *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] <= i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_EQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] == *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] == i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_NEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] != *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] != i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      default:
	go_BYE(-1);
	break;
      }
      nn_val = TRUE;
    }
    else {
      *ptr_is_any_null = true;
      cval = 0;
      nn_val = FALSE;
    }
    fwrite(&cval, 1, sizeof(char), ofp);
    fwrite(&nn_val, 1, sizeof(char), nn_ofp);
  }
 BYE:
  return(status);
}
//-----------------------------------------------------------
int
internal_f1f2opf3_cond_longlong( 
				long long *i1ptr, 
				char *nn_i1ptr,
				bool is_f2_scalar,
				long long *i2ptr, 
				char *nn_i2ptr,
				long long nR, 
				int iop,
				FILE *ofp,
				FILE *nn_ofp,
				bool *ptr_is_any_null
				)
{
  int status = 0;
  char cval, nn_val;

  *ptr_is_any_null = false;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( ( nn_i1ptr == NULL ) || ( nn_i1ptr[i] == TRUE ) ) &&
         ( ( nn_i2ptr == NULL ) || ( nn_i2ptr[i] == TRUE ) ) ) {
      switch ( iop ) {
      case IOP_GT : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] > *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] > i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_LT : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] < *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] < i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_GEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] >= *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] >= i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_LEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] <= *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] <= i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_EQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] == *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] == i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      case IOP_NEQ : 
	if ( is_f2_scalar ) { 
	  if ( i1ptr[i] != *i2ptr ) { cval = TRUE; } else { cval = FALSE; }
	}
	else {
	  if ( i1ptr[i] != i2ptr[i] ) { cval = TRUE; } else { cval = FALSE; }
	}
	break;
      default:
	go_BYE(-1);
	break;
      }
      nn_val = TRUE;
    }
    else {
      *ptr_is_any_null = true;
      cval = 0;
      nn_val = FALSE;
    }
    fwrite(&cval, 1, sizeof(char), ofp);
    fwrite(&nn_val, 1, sizeof(char), nn_ofp);
  }
 BYE:
  return(status);
}


int
internal_f1f2opf3_cond_bool( 
			    char *i1ptr, 
			    char *nn_i1ptr,
			    char *i2ptr, 
			    char *nn_i2ptr,
			    long long nR, 
			    int iop,
			    FILE *ofp,
			    FILE *nn_ofp,
			    bool *ptr_is_any_null
			    )
{
  int status = 0;
  char cval, nn_val;

  *ptr_is_any_null = false;
  for ( long long i = 0; i < nR; i++ ) { 
    if ( ( ( nn_i1ptr == NULL ) || ( nn_i1ptr[i] == TRUE ) ) &&
         ( ( nn_i2ptr == NULL ) || ( nn_i2ptr[i] == TRUE ) ) ) {
      if ( ( ( i1ptr[i] != TRUE ) && ( i1ptr[i] != FALSE ) ) || 
           ( ( i2ptr[i] != TRUE ) && ( i2ptr[i] != FALSE ) ) ) {
	go_BYE(-1);
      }
      switch ( iop ) {
      case BOP_AND : 
	if ( ( i1ptr[i] == 1 ) && ( i2ptr[i]  == 1 ) ) {
	  cval = TRUE; 
	} 
	else { 
	  cval = FALSE; 
	}
	break;
      case BOP_OR : 
	if ( ( i1ptr[i] == 1 ) || ( i2ptr[i]  == 1 ) ) {
	  cval = TRUE; 
	} 
	else { 
	  cval = FALSE; 
	}
	break;
      default:
	go_BYE(-1);
	break;
      }
      nn_val = TRUE;
    }
    else {
      *ptr_is_any_null = true;
      cval = 0;
      nn_val = FALSE;
    }
    fwrite(&cval, 1, sizeof(char), ofp);
    fwrite(&nn_val, 1, sizeof(char), nn_ofp);
  }
 BYE:
  return(status);
}
