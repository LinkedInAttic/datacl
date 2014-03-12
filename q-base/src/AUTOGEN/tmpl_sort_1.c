// START FUNC DECL
int sort___DIRECTION_____TYPE__(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  __TYPE2__ val1, val2;
  __TYPE2__ *ptr1, *ptr2;
  ptr1 = (__TYPE2__ *)ii;
  ptr2 = (__TYPE2__ *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  /* Output in __DIRECTION__ order */
  if ( val1 __COMPARATOR1__ val2 )  {
    return (1);
  }
  else if ( val1 __COMPARATOR2__ val2 ) {
    return (-1);
  }
  else {
    return(0);
  }
}
