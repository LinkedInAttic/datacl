// START FUNC DECL
void
conv_F8_to_I8( 
			    double *in, 
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

