// START FUNC DECL
void
opt_cmp_eq_mult_scalar_I8( 
			      long long *invals, 
			      long long n1, 
			      long long *hashvals, 
			      int n,
			      int a,
			      int b,
			      char * restrict out
			       )
// STOP FUNC DECL
{
  /* Assumption: in2 is sorted in ascending order */
  for ( long long i = 0; i < n1; i++ ) { 
    long long inval = invals[i];
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
