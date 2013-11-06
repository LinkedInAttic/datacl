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
#include <stdbool.h>
// START FUNC DECL
void
assign_I4(
    int *out,
    int * restrict in,
    long long n
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    int val;
    val = *in;
    *out = val;
    in++;
    out++;
  }
}


// START FUNC DECL
void
assign_const_I4(
    int *out,
    long long n,
    int val
    )
// STOP FUNC DECL
{
  for ( long long i = 0; i < n; i++ ) { 
    *out = val;
    out++;
  }
}

