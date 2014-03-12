// START FUNC DECL
void
decr_I4( 
			    int *in, 
			    int nR, 
			    int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    int  val;
    val = *in++;
    val -- ;
    *out++ = val;
  }
}

