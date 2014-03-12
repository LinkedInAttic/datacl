// START FUNC DECL
void
cmp_ge_F8( 
			    double *in1, 
			    double *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    double val1 = *in1;
    double val2 = *in2;
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
