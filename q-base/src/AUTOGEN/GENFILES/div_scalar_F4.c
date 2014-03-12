// START FUNC DECL
void div_scalar_F4(
			    float *in1, 
			    float in2, 
			    long long nR, 
			    float * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ / in2;
  }
}
