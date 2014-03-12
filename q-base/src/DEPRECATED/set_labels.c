#include <stdio.h>
#include "constants.h"
#include "macros.h"
#include <stdlib.h>
#include "set_labels.h"

#undef XXX
#ifdef XXX

/* Data structure for communication with thread safe versions.  This
 *    type is to be regarded as opaque.  It's only exported because
 *    users
 *       have to allocate objects of this type.  */
struct drand48_data {
  unsigned short int __x[3];  /* Current state.  */
  unsigned short int __old_x[3]; /* Old state.  */
  unsigned short int __c;     /* Additive const. in congruential formula.  */
  unsigned short int __init;  /* Flag for initializing.  */
  unsigned long long int __a; /* Factor in congruential formula.  */
};
extern int drand48_r(struct drand48_data *buffer, double *result);
#endif
int
set_labels(
    int *X,
    int *labels,
    long long *num_with_labels,
    int num_labels,
    long long nR
    )
{
  int status = 0;
  long long idx = 0;
  for ( int i = 0; i < num_labels; i++ ) { 
    for ( long long ctr = 0; ctr < num_with_labels[i]; ctr++ ) {
      X[idx++] = labels[i];
    }
  }
  if ( idx != nR ) { 
    fprintf(stderr, "idx = %lld, nR = %lld \n", idx, nR);
    go_BYE(-1); }
BYE:
  return(status);
}

int
permute_I4(
    int *X,
    long long n
    )
{
  int status = 0;
  struct drand48_data buffer;
  srand48_r(852187451, &buffer);
  for (long long k = n-1; k > 0; k--) {
    long long pos; int swap; double dtemp;
    drand48_r(&buffer, &dtemp);
    pos = dtemp * n; if ( pos == n ) { pos--; }
    swap   = X[pos];
    X[pos] = X[k];
    X[k]   = swap;
  }
BYE:
  return(status);
}
