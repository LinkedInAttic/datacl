// START FUNC DECL
void
nn_conv_I2_to_I4( 
			    short *in, 
			    int nR, 
			    char *nn,
			    int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (int) *in;
    }
    in++;
    out++;
  }
}

