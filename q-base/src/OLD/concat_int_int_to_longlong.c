// START FUNC DECL
void
concat_int_int_to_long(
    int *X,
    int *Y,
    int n,
    long long *Z
    )
// STOP FUNC DECL
{
  unsigned long long ltempx, ltempy;

  for ( int i = 0; i < n; i++ ) { 
    ltempx = (long long)*X++;
    ltempy = (long long)*Y++;
    ltempx = ltempx << 32; 
    *Z++ = ltempx | ltempy;
  }
}
