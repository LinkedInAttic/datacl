// START FUNC DECL
void
conv_I4_to_I2( 
			    int *in, 
			    int nR, 
			    short * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (short) *in;
    in++;
    out++;
  }
}

