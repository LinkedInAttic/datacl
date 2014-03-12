// START FUNC DECL
void
s_to_f_const_F4(
    float *X,
    const long long nX,
    float val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nX; i++ ) {
    X[i] = val;
  }
}
