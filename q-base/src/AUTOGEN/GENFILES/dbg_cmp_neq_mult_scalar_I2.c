// START FUNC DECL
void
dbg_cmp_neq_mult_scalar_I2( 
			      short *invals, 
			      long long n1, 
			      short *svals, 
			      int n_svals,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  /* TODO: Can we flip loop order to get vectorization? */
  for ( long long i = 0; i < n1; i++ ) { 
    short inval = invals[i];
    register short outval = 1;
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
