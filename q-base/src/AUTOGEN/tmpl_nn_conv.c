// START FUNC DECL
void
nn_conv___INTYPE___to___OUTTYPE__( 
			    __INTYPE2__ *in, 
			    int nR, 
			    char *nn,
			    __OUTTYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (__OUTTYPE2__) *in;
    }
    in++;
    out++;
  }
}

