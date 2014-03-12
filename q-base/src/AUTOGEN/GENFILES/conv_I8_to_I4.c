// START FUNC DECL
void
conv_I8_to_I4( 
			    long long *in, 
			    int nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (int) *in;
    in++;
    out++;
  }
}

