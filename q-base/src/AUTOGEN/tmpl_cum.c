// TODO P3: Can be parallelized.
// START FUNC DECL
void
cum___INTYPE___to___OUTTYPE__( 
			    __INTYPE2__ *in, 
			    long long nR, 
			    __OUTTYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  register __INTYPE2__ inval; 
  register __OUTTYPE2__ newval, prevval;
  *out = (__OUTTYPE2__) *in;
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

