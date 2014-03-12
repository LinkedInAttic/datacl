// START FUNC DECL
void
cmp_gt_I1( 
			    char *in1, 
			    char *in2, 
			    long long nR, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    char val1 = *in1;
    char val2 = *in2;
    if ( val1 > val2 ) { 
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
