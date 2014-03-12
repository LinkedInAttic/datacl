// START FUNC DECL
void
dbg_cmp_eq_mult_scalar___XTYPE__( 
			      __ITYPE__ *invals, 
			      long long n1, 
			      __ITYPE__ *svals, 
			      int n_svals,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  /* TODO: Can we flip loop order to get vectorization? */
//  __ITYPE__ minval = svals[0];
//  __ITYPE__ maxval = svals[n_svals-1];
  for ( long long i = 0; i < n1; i++ ) { 
    __ITYPE__ inval = invals[i];
    register __ITYPE__ outval = 0;
    // if ( ( inval >= minval ) && ( inval <= maxval ) ) {
#pragma simd
    for ( int j = 0; j < n_svals; j++ ) { 
      if ( inval == svals[j] ) { 
        outval = 1;
      }
    }
    // }
    *out = outval;
    out++;
  }
}
