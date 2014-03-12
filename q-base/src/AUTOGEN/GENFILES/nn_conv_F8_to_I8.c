// START FUNC DECL
void
nn_conv_F8_to_I8( 
			    double *in, 
			    int nR, 
			    char *nn,
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (long long) *in;
    }
    in++;
    out++;
  }
}

