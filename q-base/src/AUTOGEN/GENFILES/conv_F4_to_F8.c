// START FUNC DECL
void
conv_F4_to_F8( 
			    float *in, 
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

