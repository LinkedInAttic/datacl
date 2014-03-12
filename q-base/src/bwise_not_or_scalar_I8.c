// START FUNC DECL
void bwise_not_or_scalar_I8(
			    unsigned long long *in1, 
			    unsigned long long in2, 
			    long long nR, 
			    unsigned long long * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = ~(*in1++ | in2);
  }
}
