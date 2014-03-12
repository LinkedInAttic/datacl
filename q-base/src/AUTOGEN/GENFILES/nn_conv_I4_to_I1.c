// START FUNC DECL
void
nn_conv_I4_to_I1( 
			    int *in, 
			    int nR, 
			    char *nn,
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    if ( nn[i] == 0 ) { 
      *out = 0;
    }
    else {
      *out = (char) *in;
    }
    in++;
    out++;
  }
}

