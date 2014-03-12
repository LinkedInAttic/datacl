// START FUNC DECL
void bwise_xor_scalar_I1(
			    unsigned char *in1, 
			    unsigned char in2, 
			    long long nR, 
			    unsigned char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ ^ in2;
  }
}
