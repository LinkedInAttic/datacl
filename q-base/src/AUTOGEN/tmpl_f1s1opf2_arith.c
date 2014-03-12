// START FUNC DECL
void __OP___scalar___TYPE__(
			    __TYPE2__ *in1, 
			    __TYPE2__ in2, 
			    long long nR, 
			    __TYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    *out++ = *in1++ __OP2__ in2;
  }
}
