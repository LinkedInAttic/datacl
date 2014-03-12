#include <stdbool.h>
#include "assign_I1.h"
// START FUNC DECL
void
cmp_eq_mult_scalar_simple_I4( 
			      int *in1, 
			      long long n1, 
			      int *in2, 
			      int n2, 
			      char *out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  long long block_size = 65536; long long num_blocks;

  num_blocks = n1 / block_size;
  if ( ( num_blocks * block_size ) != n1 ) { num_blocks++; }

  for ( long long k = 0; k < num_blocks; k++ ) { 
    long long lb = k * block_size;
    long long ub = lb + block_size; 
    if ( k == num_blocks-1 ) { ub = n1; }
#ifdef IPP
    ippsSet_8u(0, out+lb, (ub-lb));
#else
    assign_const_I1(out + lb, 0, (ub-lb));
#endif
    for ( int j = 0; j < n2; j++ ) { 
      int cmpval = in2[j];
      for ( long long i = lb; i < ub; i++ ) { 
        int inval = in1[i];
        int rslt =  inval == cmpval;
        out[i] = rslt;
      }
    }
  }
}
