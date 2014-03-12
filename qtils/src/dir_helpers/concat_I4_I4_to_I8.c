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
concat_I4_I4_to_I8(
    unsigned int *X,
    unsigned int *Y,
    long long n,
    unsigned long long *Z
    )
// STOP FUNC DECL
{
  unsigned long long ultempx, ultempy;
  unsigned int       uitempx, uitempy;

  for ( int i = 0; i < n; i++ ) { 
    uitempx = *X;
    uitempy = *Y;

    ultempx = (unsigned long long)uitempx;
    ultempy = (unsigned long long)uitempy;

    *Z = (ultempx << 32) | ultempy;
    X++;
    Y++;
    Z++;
  }
}
