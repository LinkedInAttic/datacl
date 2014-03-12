// START FUNC DECL
void
conv_I4_to_F4( 
			    int *in, 
			    int nR, 
			    float * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (float) *in;
    in++;
    out++;
  }
}

