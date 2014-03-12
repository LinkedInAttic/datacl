// START FUNC DECL
void
conv_I8_to_I1( 
			    long long *in, 
			    int nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (char) *in;
    in++;
    out++;
  }
}

