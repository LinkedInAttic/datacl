#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "macros.h"

int
f1s1opf2_uint_bool(
		   unsigned int *X,
		   int nR,
		   char *op,
		   unsigned int uiscalar,
		   FILE *ofp,
		   char *str_meta_data
		   )
{
  int status = 0;
  char c_is_diff, c_is_sel;
  unsigned int uitemp;

  if ( strcmp(op, "is_prev_diff") == 0 ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( i == 0 ) {
	c_is_diff = TRUE;
      }
      else {
	if ( X[i-1] == X[i] ) {
	  c_is_diff = FALSE;
	}
	else {
	  c_is_diff = TRUE;
	}
      }
      fwrite(&c_is_diff, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "eq") == 0 ) || ( strcmp(op, "=") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] == uiscalar ) { 
	c_is_diff = TRUE;
      }
      else { 
	c_is_diff = FALSE;
      }
      fwrite(&c_is_diff, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "neq") == 0 ) || ( strcmp(op, "!=") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] == uiscalar ) { 
	c_is_diff = TRUE;
      }
      else { 
	c_is_diff = FALSE;
      }
      fwrite(&c_is_diff, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "gt") == 0 ) || ( strcmp(op, ">") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] > uiscalar ) { 
	c_is_sel = TRUE;
      }
      else { 
	c_is_sel = FALSE;
      }
      fwrite(&c_is_sel, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "lt") == 0 ) || ( strcmp(op, "<") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] > uiscalar ) { 
	c_is_sel = TRUE;
      }
      else { 
	c_is_sel = FALSE;
      }
      fwrite(&c_is_sel, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "geq") == 0 ) || ( strcmp(op, ">=") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] > uiscalar ) { 
	c_is_sel = TRUE;
      }
      else { 
	c_is_sel = FALSE;
      }
      fwrite(&c_is_sel, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "leq") == 0 ) || ( strcmp(op, "<=") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=bool:sz=1:nR=");
    for ( int i = 0; i < nR; i++ ) { 
      if ( X[i] > uiscalar ) { 
	c_is_sel = TRUE;
      }
      else { 
	c_is_sel = FALSE;
      }
      fwrite(&c_is_sel, sizeof(char), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "add") == 0 ) || ( strcmp(op, "+") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=uint:sz=4");
    for ( int i = 0; i < nR; i++ ) { 
      uitemp = X[i] + uiscalar;
      fwrite(&uitemp, sizeof(unsigned int), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "sub") == 0 ) || ( strcmp(op, "-") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=uint:sz=4");
    for ( int i = 0; i < nR; i++ ) { 
       uitemp = X[i] - uiscalar;
      fwrite(&uitemp, sizeof(unsigned int), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "mul") == 0 ) || ( strcmp(op, "*") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=uint:sz=4");
    for ( int i = 0; i < nR; i++ ) { 
      uitemp = X[i] * uiscalar;
      fwrite(&uitemp, sizeof(unsigned int), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "div") == 0 ) || ( strcmp(op, "/") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=uint:sz=4");
    for ( int i = 0; i < nR; i++ ) { 
      uitemp = X[i] / uiscalar;
      fwrite(&uitemp, sizeof(unsigned int), 1, ofp);
    }
  }
  else if ( ( strcmp(op, "rem") == 0 ) || ( strcmp(op, "%") == 0 ) ) {
    strcpy(str_meta_data, "fldtype=uint:sz=4");
    for ( int i = 0; i < nR; i++ ) { 
      uitemp = X[i] % uiscalar;
      fwrite(&uitemp, sizeof(unsigned int), 1, ofp);
    }
  }
  else { go_BYE(-1); }
 BYE:
  return(status);
}
