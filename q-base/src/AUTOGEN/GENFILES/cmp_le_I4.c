// START FUNC DECL
void
cmp_le_I4( 
			    int *in1, 
			    int *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    int val1 = *in1;
    int val2 = *in2;
    if ( val1 <= val2 ) { 
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
