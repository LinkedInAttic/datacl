// START FUNC DECL
void mul_scalar_F8(
			    double *in1, 
			    double in2, 
			    long long nR, 
			    double * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ * in2;
  }
}
