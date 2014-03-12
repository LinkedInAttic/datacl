// START FUNC DECL
void
s_to_f_seq___XTYPE__(
    __ITYPE__ *X,
    const long long nX,
    const __ITYPE__ start,
    const __ITYPE__ incr
    )
// STOP FUNC DECL
{
  __ITYPE__ val = start;
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
    val += incr;
  }
}
