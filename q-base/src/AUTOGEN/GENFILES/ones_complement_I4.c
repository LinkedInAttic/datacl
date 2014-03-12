// START FUNC DECL
void
ones_complement_I4( 
			    int *in, 
			    int nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = ~ (*in); in++;
  }
}

