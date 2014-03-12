// START FUNC DECL
void
nn_conv_F8_to_I4( 
			    double *in, 
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

