// START FUNC DECL
void
cmp_gt_and_lt_I2( 
			    short *in1, 
			    long long nR, 
			    short s1, 
			    short s2, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  short inval;
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
