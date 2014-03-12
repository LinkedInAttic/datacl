// START FUNC DECL
void
conv_I1_to_I4( 
			    char *in, 
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

