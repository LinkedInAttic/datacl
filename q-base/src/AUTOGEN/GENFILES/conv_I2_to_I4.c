// START FUNC DECL
void
conv_I2_to_I8( 
			    short *in, 
			    int nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (long long) *in;
    in++;
    out++;
  }
}

