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
typedef long long long_long;
#include "f1f2opf3_arith.h"
//-----------------------------------------------------------
int
f1f2opf3_arith_{FLDTYPE}( 
			    FLDTYPE *i1ptr, 
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
