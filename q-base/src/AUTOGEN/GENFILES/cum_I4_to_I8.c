// TODO P3: Can be parallelized.
// START FUNC DECL
void
cum_I4_to_I8( 
			    int *in, 
			    long long nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  register int inval; 
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

