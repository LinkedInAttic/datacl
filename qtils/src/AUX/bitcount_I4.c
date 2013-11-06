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
#include <stdio.h>
#define BITS_PER_BYTE 8
// START FUNC DECL
void
bitcount_I4( 
		    int *in, 
		    int nR, 
		    int * restrict out
		    )
// STOP FUNC DECL
{
  unsigned int temp, mask = 1;
  int irslt = 0, iters = BITS_PER_BYTE * sizeof(int);
    for ( long long i = 0; i < nR; i++ ) { 
      temp = (unsigned int) *in;
      irslt = 0;
      for ( int j = 0; j < iters; j++ ) { 
	irslt += temp & mask;
	temp = temp >> 1; 
      }
      *out = irslt;
      in++;
      out++;
    }
}
