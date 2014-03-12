#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "assign_I1.h"
#include "assign_I2.h"
#include "assign_I4.h"
#include "assign_I8.h"
#include "mix.h"

/* Note that current impementation assumes that sqr(num_scalars) < 65536 */
/* We use short for hashes */

// START FUNC DECL
int
mk_hashes_I8(
	     long long *scalars,
	     int num_scalars,
	     long long **ptr_hashes,
	     unsigned int *ptr_n,
	     unsigned int *ptr_a,
	     unsigned int *ptr_b
	     )
// STOP FUNC DECL
{
  int status = 0;
  long long *hashes = NULL; 
  unsigned char *used = NULL;
  unsigned int n, a, b;

  *ptr_n = *ptr_a = *ptr_b = 0; *ptr_hashes = NULL;
  if ( ( num_scalars < 1 ) || ( num_scalars > MAX_NUM_SCALARS ) ){ go_BYE(-1);}
  n = prime_geq(num_scalars * num_scalars);
  if ( n <= 1 ) { go_BYE(-1); }
  hashes = malloc(n * sizeof(long long));
  return_if_malloc_failed(hashes);
  used = malloc(n * sizeof(unsigned char));
  return_if_malloc_failed(used);

  srand48(get_time_usec());
  for ( int iter = 0; ; iter++ ) { 
    // initializations 
    a = (unsigned int)mrand48();
    a = prime_geq(a);
    b = (unsigned int)mrand48();
    b = prime_geq(b);
    bool is_collision = false;
    assign_const_I8( hashes, n, 0);
    assign_const_I1((char *)used,   n, 0);
    //-------------------------------------
    for ( int i = 0; i < num_scalars; i++ ) { 
      unsigned long long val = scalars[i]; 
      unsigned long long ltemp = ( ( a * val ) + b ) % n;
      unsigned short loc = (unsigned short)ltemp;
      if ( used[loc] == 0 ) { 
	used[loc] = 1;
	hashes[loc] = scalars[i];
      }
      else {
	is_collision = true;
	break;
      }
    }
    if ( is_collision == false ) {  break; }
  }
  int chk_cnt = 0;
  for ( int i = 0; i < n; i++ ) { 
    if ( used[i] == 1 ) { chk_cnt++; }
    if ( ( used[i] == 0 ) && ( hashes[i] != 0 ) )  { go_BYE(-1); }

  }
  if ( chk_cnt != num_scalars ) { go_BYE(-1); }

  //---------------------------------------
  *ptr_hashes = hashes;
  *ptr_a = a;
  *ptr_b = b;
  *ptr_n = n;
 BYE:
  free_if_non_null(used);
  return(status);
}
