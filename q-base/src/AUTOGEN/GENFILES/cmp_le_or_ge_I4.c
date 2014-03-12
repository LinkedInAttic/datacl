// START FUNC DECL
void
cmp_le_or_ge_I4( 
			    int *in1, 
			    long long nR, 
			    int s1, 
			    int s2, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  int inval;
  char outval;
  for ( long long i = 0; i < nR; i++ ) { 
    inval = *in1;
    if ( ( inval <= s1 ) || ( inval >= s2 ) ) {
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
