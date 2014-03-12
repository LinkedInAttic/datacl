// TODO P3: Can be parallelized.
// START FUNC DECL
void
cum_I1_to_I4( 
			    char *in, 
			    long long nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  register char inval; 
  register int newval, prevval;
  *out = (int) *in;
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

