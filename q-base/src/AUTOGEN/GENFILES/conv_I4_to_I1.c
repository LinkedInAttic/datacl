// START FUNC DECL
void
conv_I4_to_I1( 
			    int *in, 
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

