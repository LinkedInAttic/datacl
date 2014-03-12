#include <stdio.h>
#include "macros.h"
// START FUNC DECL
int
pr_fld_SC(
    char *X,
    int len,
    long long lb,
    long long ub,
    const char *cfld_X,
    FILE *ofp
    )
// STOP FUNC DECL
{
  int status = 0;
  if ( X == NULL ) { go_BYE(-1); }
  if ( len <=  1 ) { go_BYE(-1); }
  for ( long long i = lb; i < ub; i++ ) { 
    if ( ( cfld_X != NULL ) && ( cfld_X[i] == 0 ) ) {
      continue;
    }
    char *cptr = X + (i*(len+1));
    fprintf(ofp, "\"");
    for ( int j = 0; ( ( j < len ) && ( *cptr != '\0' ) ) ; ) {
      fprintf(ofp, "%c", *cptr++);
    }
    fprintf(ofp, "\"");
    fprintf(ofp, "\n");
  }
BYE:
  return(status);
}
