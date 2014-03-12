// START FUNC DECL
void add_scalar_I1(
			    char *in1, 
			    char in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ + in2;
  }
}
