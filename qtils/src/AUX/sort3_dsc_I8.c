/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
// START FUNC DECL
int sort3_dsc_I8(
				 const void *ii, 
				 const void *jj
				 )
// STOP FUNC DECL
{ 
  long long val11, val12, val13, val21,val22, val23;
  long long *ptr1, *ptr2;
  ptr1 = (long long *)ii;
  ptr2 = (long long *)jj;

  val11 = *ptr1++; /* first element of first value */
  val12 = *ptr1++; /* second element of first value */
  val13 = *ptr1++; /* third element of first value */
  val21 = *ptr2++; /* first element of second value */
  val22 = *ptr2++; /* second element of second value */
  val23 = *ptr2++; /* third element of second value */

  /* Output in dsc order */
  if ( val11 < val21 )  {
    return (1);
  }
  else if ( val11 > val21 ) {
    return (-1);
  }
  else {
    if ( val12 < val22 )  {
      return (1);
    }
    else if ( val12 > val22 ) {
      return (-1);
    }
    else {
      if ( val13 < val23 )  {
	return (1);
      }
      else if ( val13 > val23 ) {
	return (-1);
      }
      else {
	return(0);
      }
    }
  }
}
