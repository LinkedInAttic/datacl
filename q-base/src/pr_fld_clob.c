#include <stdio.h>
// START FUNC DECL
int
pr_fld_clob(
    char *X,
    int len,
    long long lb,
    long long ub,
    const char *nn_X,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      continue;
    }
    if ( ( nn_X != NULL ) && ( nn_X[i] == 0 ) ) {
      fprintf(ofp, "\"\"\n");
      continue;
    }
    char *cptr = X + (i*len);
    fprintf(ofp, "\"");
    for ( int j = 0; ( ( j < len ) && ( *cptr != '\0' ) ) ; ) {
      fprintf(ofp, "%c", *cptr++);
    }
    fprintf(ofp, "\"");
    fprintf(ofp, "\n");
  }
  return(status);
}
