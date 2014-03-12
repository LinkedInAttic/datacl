// START FUNC DECL
void
conv___INTYPE___to___OUTTYPE__( 
			    __INTYPE2__ *in, 
			    int nR, 
			    __OUTTYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (__OUTTYPE2__) *in;
    in++;
    out++;
  }
}

