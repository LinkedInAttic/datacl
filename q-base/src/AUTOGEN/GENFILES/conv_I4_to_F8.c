// START FUNC DECL
void
conv_I4_to_F8( 
			    int *in, 
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

