// START FUNC DECL
void
concat_I4_I4_to_I8(
    unsigned int *X,
    unsigned int *Y,
    long long n,
    unsigned long long *Z
    )
// STOP FUNC DECL
{
  unsigned long long ultempx, ultempy;
  unsigned int       uitempx, uitempy;

  for ( long long i = 0; i < n; i++ ) { 
    uitempx = *X;
    uitempy = *Y;

    ultempx = (unsigned long long)uitempx;
    ultempy = (unsigned long long)uitempy;

    *Z = (ultempx << 32) | ultempy;
    X++;
    Y++;
    Z++;
  }
}
