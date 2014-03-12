// START FUNC DECL
void
f_to_s___XOP_____XTYPE__(
    __ITYPE__ *X,
    long long nX,
    __ITYPE__ *ptr_rslt
    )
// STOP FUNC DECL
{
  __ITYPE__ rslt;
  rslt = *ptr_rslt;
  for ( long long i = 0; i < nX; i++ ) { 
    if ( X[i] __IOP__ rslt ) {
      rslt = X[i];
    }
  }
  *ptr_rslt = rslt;
}
    
