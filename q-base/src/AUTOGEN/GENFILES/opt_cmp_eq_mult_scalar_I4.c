// START FUNC DECL
void
opt_cmp_eq_mult_scalar_I4( 
			      int *invals, 
			      long long n1, 
			      int *hashvals, 
			      int n,
			      int a,
			      int b,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    int inval = invals[i];
    unsigned long long ltemp = ( inval * a ) + b;
    int loc = ltemp % n;
    if ( hashvals[loc] == inval ) { 
      *out = 1;
    }
    else {
      *out = 0;
    }
    out++;
  }
}
