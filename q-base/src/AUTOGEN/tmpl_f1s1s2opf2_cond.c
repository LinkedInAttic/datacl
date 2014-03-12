// START FUNC DECL
void
cmp___XCMP1_____XBOP_____XCMP2_____XTYPE__( 
			    __ITYPE__ *in1, 
			    long long nR, 
			    __ITYPE__ s1, 
			    __ITYPE__ s2, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  __ITYPE__ inval;
  char outval;
  for ( long long i = 0; i < nR; i++ ) { 
    inval = *in1;
    if ( ( inval __ICMP1__ s1 ) __IBOP__ ( inval __ICMP2__ s2 ) ) {
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
