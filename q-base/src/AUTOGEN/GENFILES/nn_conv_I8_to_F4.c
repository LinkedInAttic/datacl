// START FUNC DECL
void
nn_conv_I8_to_F4( 
			    long long *in, 
			    int nR, 
			    char *nn,
			    float * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (float) *in;
    }
    in++;
    out++;
  }
}

