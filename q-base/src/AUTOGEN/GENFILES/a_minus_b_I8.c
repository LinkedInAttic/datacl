// START FUNC DECL
void
a_minus_b_I8(
	     long long *f1, 
	     long long nR1, 
	     long long *f2, 
	     long long nR2, 
	     long long *f3, 
	     long long *ptr_nR3
	     )
// STOP FUNC DECL
{
  long long nR3 = 0;
  long long f1I4val, f2I4val;
  long long *endf1= f1 + nR1;
  long long *endf2= f2 + nR2;
  for ( ; ( ( f1 != endf1 ) || ( f2 != endf2 ) ); ) {
    if ( f2 == endf2 ) { /* copy items from f1 */
      f1I4val = *f1;
      *f3 = f1I4val;
      f3++;
      f1++;
      nR3++;
    }
    else if ( f1 == endf1 ) { /* nothing more to do */
      break; 
    }
    else {
      f1I4val = *f1;
      f2I4val = *f2;
      if ( f1I4val < f2I4val ) { 
	*f3 = f1I4val;
	f1++;
	f3++;
	nR3++;
      }
      else if ( f1I4val == f2I4val ) { 
	/* Do not copy this value into A since it is in B */
	f1++;
	f2++;
      }
      else { /* advance pointer over this element */
	f2++;
      }
    }
  }
  *ptr_nR3 = nR3;
}
