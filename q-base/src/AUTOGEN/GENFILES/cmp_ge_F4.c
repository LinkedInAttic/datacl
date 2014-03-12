// START FUNC DECL
void
cmp_ge_F4( 
			    float *in1, 
			    float *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    float val1 = *in1;
    float val2 = *in2;
    if ( val1 >= val2 ) { 
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
