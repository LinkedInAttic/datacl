#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"


typedef struct f1type {
  long long f1;
  long long flnk;
} F1TYPE;

typedef struct f2type {
  long long flnk;
  long long f2;
} F2TYPE;
static int
bin_search_ll(
	   F2TYPE *X,
	   long long nX,
	   long long key,
	   long long *ptr_lb,
	   long long *ptr_ub
	   )
// STOP FUNC DECL
{
  int status = 0;
  long long pos, lb, ub, mid;

  if ( X == NULL ) go_BYE(-1);
  *ptr_lb = *ptr_ub = -1;
  if ( nX == 1 ) { 
    if ( X[0].flnk == key ) { 
      *ptr_lb = 0; 
      *ptr_ub = 1;
    }
    else {
      *ptr_lb = -1;
      *ptr_ub = -1;
    }
    return(status); 
  }

  lb = 0; ub = nX - 1;
  pos = -1;
  for ( ; lb <= ub ; ) { 
    mid = ( lb + ub )  / 2 ;
    if ( X[mid].flnk == key ) { 
      pos = mid;
      break;
    }
    else if ( key < X[mid].flnk ) { /* reduce ub */
      ub = min(ub - 1, mid);
    }
    else /* key > X[mid] */ { /* increase lb */
      lb = max(lb + 1, mid);
    }
  }
  // Now stretch on both sides to find range of values with this key
  if ( pos >= 0 ) {
    for ( lb = pos ; lb >= 0 ; lb-- ) {
      if ( X[lb].flnk != key ) { 
	lb++;
	break;
      }
    }
    if ( lb < 0 ) { lb = 0; } 
    for ( ub = pos ; ub < nX ; ub++ ) {
      if ( X[ub].flnk != key ) { 
	break;
      }
    }
    if ( ub > nX ) { ub = nX; }
  }
  else {
    lb = ub = -1;
  }
  *ptr_lb = lb;
  *ptr_ub = ub;
 BYE:
  return(status);
}

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *t1file = NULL, *t2file = NULL, *opfile = NULL;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  F1TYPE *T1 = NULL;
  F2TYPE *T2 = NULL;
  long long f1, f2, lnk1;
  FILE *ofp = NULL;
  long long nR1, nR2, lb, ub;

  if ( argc != 4 ) {
    fprintf(stderr, "Usage is %s <tbl1> <tbl2> <optbl> \n", argv[0]); 
    go_BYE(-1); 
  }
  t1file = argv[1];
  t2file = argv[2];
  opfile = argv[3];
  if ( strcmp(t1file, t2file) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2file, opfile) == 0 ) { go_BYE(-1); }

  ofp = fopen(opfile, "wb");
  return_if_fopen_failed(ofp,  opfile, "wb");
  // mmap first file 
  status = rs_mmap(t1file, &X1, &nX1, 0);
  cBYE(status);
  nR1 = nX1 / sizeof(F1TYPE);
  if ( nR1 * sizeof(F1TYPE) != nX1 ) { go_BYE(-1); }
  if ( nR1 < 1 ) { go_BYE(-1); }
  T1 = (F1TYPE *)X1;

  // mmap first file 
  status = rs_mmap(t2file, &X2, &nX2, 0);
  cBYE(status);
  nR2 = nX2 / sizeof(F2TYPE);
  if ( nR2 * sizeof(F2TYPE) != nX2 ) { go_BYE(-1); }
  if ( nR2 < 1 ) { go_BYE(-1); }
  T2 = (F2TYPE *)X2;

  for ( long long i = 0; i < nR1; i++ ) { 
    f1   = T1[i].f1;
    lnk1 = T1[i].flnk;
    status = bin_search_ll(T2, nR2, lnk1, &lb, &ub);
    cBYE(status);
    if ( ( lb < 0 ) || ( ub < 0 ) ) {
      // TODO SHOULD THIS SITUATION BE ALLOWED TO OCCUR ?
      // fprintf(stderr, "Could not find clean title id for rthash = %lld \n", lnk1);
      continue;
    }
    for ( long long j = lb; j < ub; j++ ) { 
      f2   = T2[j].f2;
      fwrite(&f1, sizeof(long long), 1, ofp);
      fwrite(&f2, sizeof(long long), 1, ofp);
    }
  }

BYE:
  fclose_if_non_null(ofp);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  return(status);
}
// START FUNC DECL
