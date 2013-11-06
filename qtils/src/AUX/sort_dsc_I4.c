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
int sort_dsc_I4(
		       const void *ii, 
		       const void *jj
		       )
// STOP FUNC DECL
{ 
  int val1, val2;
  int *ptr1, *ptr2;
  ptr1 = (int *)ii;
  ptr2 = (int *)jj;
  val1 = *ptr1;
  val2 = *ptr2;

  /* Output in dsc order */
  if ( val1 < val2 )  {
    return (1);
  }
  else if ( val1 > val2 ) {
    return (-1);
  }
  else {
    return(0);
  }
}
