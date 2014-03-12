// START FUNC DECL
void shift_right_scalar_UI4(
			    unsigned int *in1, 
			    unsigned int in2, 
			    long long nR, 
			    unsigned int * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ >> in2;
  }
}
