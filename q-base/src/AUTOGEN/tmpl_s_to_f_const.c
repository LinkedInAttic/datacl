// START FUNC DECL
void
s_to_f_const___XTYPE__(
    __ITYPE__ *X,
    const long long nX,
    __ITYPE__ val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
  }
}
