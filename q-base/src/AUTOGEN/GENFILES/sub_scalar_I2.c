// START FUNC DECL
void sub_scalar_I2(
			    short *in1, 
			    short in2, 
			    long long nR, 
			    short * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ - in2;
  }
}
