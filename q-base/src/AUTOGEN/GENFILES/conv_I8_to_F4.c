// START FUNC DECL
void
conv_I8_to_F4( 
			    long long *in, 
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

