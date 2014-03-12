// START FUNC DECL
void shift_right_scalar_UI8(
			    unsigned long long *in1, 
			    unsigned long long in2, 
			    long long nR, 
			    unsigned long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ >> in2;
  }
}
