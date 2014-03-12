// START FUNC DECL
void
s_to_f_const_I1(
    char *X,
    const long long nX,
    char val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
  }
}
