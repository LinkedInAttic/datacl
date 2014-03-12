// TODO P3: Can be parallelized.
// START FUNC DECL
void
cum_I8_to_I8( 
			    long long *in, 
			    long long nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  register long long inval; 
  register long long newval, prevval;
  *out = (long long) *in;
  prevval = *out;
  in++;
  out++;
  for ( long long i = 1; i < nR; i++ ) { 
    inval = *in;
    newval = inval + prevval;
    *out  = newval;
    out++;
    in++;
    prevval = newval;
  }
}

