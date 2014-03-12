#include <stdio.h>
// START FUNC DECL
int
pr_fld___XTYPE__(
    __ITYPE__ *X,
    long long lb,
    long long ub,
    const char *nn_X,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  __ITYPE__ val;
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      continue;
    }
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) {
      fprintf(ofp, "\"\"\n");
      continue;
    }
    val = X[i];
    if ( nn_X == NULL ) { 
      fprintf(ofp, "__FORMAT__\n", val);
    }
    else {
      fprintf(ofp, "\"__FORMAT__\"\n", val);
    }
  }
  return(status);
}
