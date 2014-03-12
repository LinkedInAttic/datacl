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
void
cmp_le_or_ge_I4( 
			    int *in1, 
			    long long nR, 
			    int s1, 
			    int s2, 
			    char * restrict out
			    )
// STOP FUNC DECL
{
  int inval;
  char outval;
  for ( long long i = 0; i < nR; i++ ) { 
    inval = *in1;
    if ( ( inval <= s1 ) || ( inval >= s2 ) ) {
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
