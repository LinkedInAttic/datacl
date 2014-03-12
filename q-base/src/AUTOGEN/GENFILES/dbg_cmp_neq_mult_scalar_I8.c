// START FUNC DECL
void
dbg_cmp_neq_mult_scalar_I8( 
			      long long *invals, 
			      long long n1, 
			      long long *svals, 
			      int n_svals,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  /* TODO: Can we flip loop order to get vectorization? */
  for ( long long i = 0; i < n1; i++ ) { 
    long long inval = invals[i];
    register long long outval = 1;
#pragma simd
    for ( int j = 0; j < n_svals; j++ ) { 
      if ( inval == svals[j] ) { 
        outval = 0;
      }
    }
    *out = outval;
    out++;
  }
}
