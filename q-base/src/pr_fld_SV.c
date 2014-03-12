#include <stdio.h>
// START FUNC DECL
int
pr_fld_SV(
    char *X,
    short *lenI2ptr,
    long long *offI8ptr,
    long long lb,
    long long ub,
    const char *nn_X,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  char *in_X = NULL;
  for ( long long i = lb; i < ub; i++ ) { 
    in_X = X + offI8ptr[i];
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      // skip over this entry 
      continue;
    }
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) {
      fprintf(ofp, "\"\"\n");
      continue;
    }
    fprintf(ofp, "\"");
    char *cptr = in_X;
    for ( int j = 0; j < lenI2ptr[i]; j++ ) { 
      if ( ( *cptr == '\\' ) || ( *cptr == '"' ) ) { 
        fprintf(ofp, "\\");
      }
      fprintf(ofp, "%c", *cptr++);
    }
    fprintf(ofp, "\"");
    fprintf(ofp, "\n");
  }
  return(status);
}
