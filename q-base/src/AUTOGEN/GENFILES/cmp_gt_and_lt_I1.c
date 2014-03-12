// START FUNC DECL
void
cmp_gt_and_lt_I1( 
			    char *in1, 
			    long long nR, 
			    char s1, 
			    char s2, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  char inval;
  char outval;
  for ( long long i = 0; i < nR; i++ ) { 
    inval = *in1;
    if ( ( inval > s1 ) && ( inval >= s2 ) ) {
      outval = 1;
    }
    else {
      outval = 0;
    }
    *out = outval;
    in1++;
    out++;
  }
}
