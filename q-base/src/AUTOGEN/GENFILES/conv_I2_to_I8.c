// START FUNC DECL
void
conv_I2_to_I4( 
			    short *in, 
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

