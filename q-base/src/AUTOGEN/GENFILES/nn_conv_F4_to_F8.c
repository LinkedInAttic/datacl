// START FUNC DECL
void
nn_conv_F4_to_F8( 
			    float *in, 
			    int nR, 
			    char *nn,
			    double * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (double) *in;
    }
    in++;
    out++;
  }
}

