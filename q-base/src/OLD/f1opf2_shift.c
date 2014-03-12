#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "hash_string.h"
#include "dbauxil.h"
#include "f1opf2_shift.h"

// START FUNC DECL
int
pr_cell(
	int ifldtype,
	long long lltemp,
	int itemp,
	char ctemp,
	char nn,
	FILE *ofp,
	FILE *nn_ofp
	)
// STOP FUNC DECL
{
  int status = 0;
  switch ( ifldtype ) { 
  case FLDTYPE_INT: 
    fwrite(&itemp, 1, sizeof(int), ofp);
    break;
  case FLDTYPE_LONGLONG: 
    fwrite(&lltemp, 1, sizeof(long long), ofp);
    break;
  case FLDTYPE_CHAR: 
    fwrite(&ctemp, 1, sizeof(char), ofp);
    break;
  default : 
    go_BYE(-1);
    break;
  }
  fwrite(&nn, 1, sizeof(char), nn_ofp);
 BYE:
  return(status);
}
//---------------------------------------------------------------
// START FUNC DECL
int 
f1opf2_shift(
	     char *X,
	     char *nn_X,
	     int nR,
	     char *fldtype,
	     int shift_by,
	     FILE *ofp,
	     FILE *nn_ofp
	     )
// STOP FUNC DECL
{
  int status = 0;
  int *iptr = NULL; char *cptr = NULL; long long *llptr = NULL;
  int itemp = 0; char ctemp = 0; long long lltemp = 0; char nn;
  int ifldtype;

  iptr = (int *)X;
  cptr = (char *)X;
  llptr = (long long *)X;
  if ( ( shift_by >= nR ) || ( shift_by <= (-1*nR) ) ) { 
    go_BYE(-1); 
  }
  status = mk_ifldtype(fldtype, &ifldtype);
  cBYE(status);

  if ( shift_by == 0 ) {
    fprintf(stderr, "NOT IMPLEMENTED\n"); go_BYE(-1);
  }
  if ( shift_by > 0 ) {
    for ( int i = 0; i < nR; i++ ) { 
      if ( i < shift_by ) {
	lltemp = itemp = 0; ctemp = 0; nn = FALSE;
      }
      else {
	if ( nn_X == NULL ) { nn = TRUE; } else { nn = nn_X[i]; }
	switch ( ifldtype ) { 
	case FLDTYPE_INT: 
	  itemp = iptr[i - shift_by];
	  break;
	case FLDTYPE_LONGLONG: 
	  lltemp = llptr[i - shift_by];
	  break;
	case FLDTYPE_CHAR: 
	  ctemp = cptr[i - shift_by];
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
      status = pr_cell(ifldtype, lltemp, itemp, ctemp, nn, ofp, nn_ofp);
      cBYE(status);
    }
  }
  else {
    shift_by *= -1;
    for ( int i = 0; i < nR; i++ ) { 
      if ( i >= (nR - shift_by) ) {
        lltemp = itemp = 0; ctemp = 0; nn = FALSE;
      }
      else {
	if ( nn_X == NULL ) { nn = TRUE; } else { nn = nn_X[i]; }
	switch ( ifldtype ) { 
	case FLDTYPE_INT: 
	  itemp = iptr[i + shift_by];
	  break;
	case FLDTYPE_LONGLONG: 
	  lltemp = llptr[i + shift_by];
	  break;
	case FLDTYPE_CHAR: 
	  ctemp = cptr[i + shift_by];
	  break;
	default : 
	  go_BYE(-1);
	  break;
	}
      }
      status = pr_cell(ifldtype, lltemp, itemp, ctemp, nn, ofp, nn_ofp);
      cBYE(status);
    }
  }
BYE:
  return(status);
}
