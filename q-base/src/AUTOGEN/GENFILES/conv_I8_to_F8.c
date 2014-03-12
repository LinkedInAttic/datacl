// START FUNC DECL
void
conv_I8_to_F8( 
			    long long *in, 
			    int nR, 
			    double * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out = (double) *in;
    in++;
    out++;
  }
}

