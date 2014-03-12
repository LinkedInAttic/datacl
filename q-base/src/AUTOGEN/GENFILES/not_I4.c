// START FUNC DECL
void
not_I4( 
			    int *in, 
			    int nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = ! (*in); in++;
  }
}

