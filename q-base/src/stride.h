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
extern int
stride_I4(
    int *f1,
    long long nR1,
    long long start, 
    long long stride, 
    int *f2,
    long long nR2
    )
;
//----------------------------
extern int
stride_I8(
    long long *f1,
    long long nR1,
    long long start, 
    long long stride, 
    long long *f2,
    long long nR2
    )
;
//----------------------------
extern int 
ext_stride(
	   const char *t1,
	   const char *f1,
	   char *str_start,
	   char *str_stride,
	   const char *t2,
	   const char *f2,
	   char *str_nR2
    )
;
//----------------------------
