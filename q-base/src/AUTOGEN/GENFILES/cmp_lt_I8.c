// START FUNC DECL
void
cmp_lt_I8( 
			    long long *in1, 
			    long long *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    long long val1 = *in1;
    long long val2 = *in2;
    if ( val1 < val2 ) { 
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
