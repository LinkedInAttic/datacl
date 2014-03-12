// START FUNC DECL
void
cum_reset___INTYPE___to___OUTTYPE__( 
			    __INTYPE2__ *in, 
			    long long nR, 
			    __INTYPE2__ reset_on,
			    __INTYPE2__ reset_to,
			    __OUTTYPE2__ *out
			    )
// STOP FUNC DECL
{
  __INTYPE2__  inval, newval, prevval;
  // __OUTTYPE2__ *bak_out = out; /* for debugging */
  // __INTYPE2__  *bak_in = in;  /* for debugging */
  inval = *in;
  if ( inval == reset_on ) { 
    *out = prevval = reset_to;
  }
  else {
    *out = prevval = inval;
  }
  in++;
  out++;
  for ( long long i = 1; i < nR; i++ ) { 
    inval = *in++;
    if ( inval == reset_on ) { 
      newval = reset_to;
    }
    else {
      newval = inval + prevval;
    }
    *out  = (__OUTTYPE2__) newval;
    out++;
    prevval = newval;
  }
}

