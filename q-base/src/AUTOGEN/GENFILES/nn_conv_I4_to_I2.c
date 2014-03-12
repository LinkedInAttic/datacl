// START FUNC DECL
void
nn_conv_I4_to_I2( 
			    int *in, 
			    int nR, 
			    char *nn,
			    short * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (short) *in;
    }
    in++;
    out++;
  }
}

