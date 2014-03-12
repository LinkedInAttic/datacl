// START FUNC DECL
void
cmp___OP_____INTYPE__( 
			    __INTYPE2__ *in1, 
			    __INTYPE2__ *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    __INTYPE2__ val1 = *in1;
    __INTYPE2__ val2 = *in2;
    if ( val1 __OP2__ val2 ) { 
      *out = 1;
    }
    else {
      *out = 0;
    }
    in1++;
    in2++;
    out++;
  }
}
