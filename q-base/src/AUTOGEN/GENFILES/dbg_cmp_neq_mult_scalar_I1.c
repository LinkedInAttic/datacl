// START FUNC DECL
void
dbg_cmp_neq_mult_scalar_I1( 
			      char *invals, 
			      long long n1, 
			      char *svals, 
			      int n_svals,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  /* TODO: Can we flip loop order to get vectorization? */
  for ( long long i = 0; i < n1; i++ ) { 
    char inval = invals[i];
    register char outval = 1;
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
