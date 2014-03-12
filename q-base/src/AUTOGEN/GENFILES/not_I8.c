// START FUNC DECL
void
not_I8( 
			    long long *in, 
			    int nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = ! (*in); in++;
  }
}

