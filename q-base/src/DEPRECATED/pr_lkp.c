#include "macros.h"
#include "qtypes.h"

int
pr_lkp(
    char *t1,
    char *t2idx,
    char *t2,
    char *t2f1,
    char *t2f2,
    char *str_len
    )
{
  int status = 0;
  int len =0;
  char *endptr; 
  long long nR1 = 0, nR2 = 0;
  int  t1f1valI4,  t2f1valI4;
  int *t1f1ptrI4, *t2f1ptrI4;
  char *X = NULL; size_t nX = 0;
  char *Y = NULL; size_t nY = 0; 
  char *Z = NULL; size_t nZ = 0; char *bak_Z = NULL;
  char dquote = '"';
  char eoln   = '\n';
  char bslash = '\\';

  if ( ( str_len == NULL ) || ( *str_len == '\0' ) )  { go_BYE(-1); }
  len = strtoll(str_len, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( len <= 1 ) { go_BYE(-1); }

  bak_Z = Z;
  t1f1ptrI4 = (int *)X;
  t2f1ptrI4 = (int *)Y;
  int j = 0;
  for ( long long i = 0; i < nR1; i++ ) { 
    t1f1valI4 = t1f1ptrI4[i];
    for ( ; j < nR2; ) { 
      t2f1valI4 = t2f1ptrI4[j];
      if ( t1f1valI4 < t2f1valI4 ) {
	Y += len;
	j++;
	continue;
      }
      if ( t1f1valI4 >= t2f1valI4 ) {
	printf("%c", dquote); /* open quote */
        if ( t1f1valI4 == t2f1valI4 ) {
	  for ( int i = 0; i < len; i++ ) {
	    if ( Y[i] == '\0' ) { break; }
	    if ( Y[i] == '\\' ) { printf("%c", bslash); }
	    if ( Y[i] == '"'  ) { printf("%c", bslash); }
	    printf("%c", Y[i]);
	  }
	}
	printf("%c", dquote); /* close quote */
	printf("%c", eoln);   /* move to next record */
      }
      Y += len;
      
    }
  }
BYE:
  rs_munmap(X,     nX);
  rs_munmap(Y,     nY);
  rs_munmap(bak_Z, nZ);
  return(status);
}
