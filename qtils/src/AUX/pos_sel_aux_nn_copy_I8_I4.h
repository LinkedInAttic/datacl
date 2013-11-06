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
pos_sel_aux_nn_copy_I8_I4(
    long long *f1_select, 
    char *nn_f1_select,
    long long *f1_copy,
    char *nn_f1_copy,
    long long nR1, 
    int *f2, 
    char *nn_f2,
    long long nR2, 
    long long lb,
    long long ub,
    long long *f3,
    long long *ptr_cnt
    )
;

