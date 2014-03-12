// START FUNC DECL
void shift_left_scalar_I8(
			    long long *in1, 
			    long long in2, 
			    long long nR, 
			    long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ << in2;
  }
}
