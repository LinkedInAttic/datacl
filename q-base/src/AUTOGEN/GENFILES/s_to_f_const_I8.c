// START FUNC DECL
void
s_to_f_const_I8(
    long long *X,
    const long long nX,
    long long val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
  }
}
