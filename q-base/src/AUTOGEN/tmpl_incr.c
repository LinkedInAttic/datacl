// START FUNC DECL
void
__UNARYOP_____INTYPE__( 
			    __INTYPE2__ *in, 
			    int nR, 
			    __INTYPE2__ * restrict out
			    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < nR; i++ ) { 
    __INTYPE2__  val;
    val = *in++;
    val __UNARYOP2__ ;
    *out++ = val;
  }
}

