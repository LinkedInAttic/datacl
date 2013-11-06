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
rd_dict(
    const unsigned long long *hashvals, /* input dictionary. Part 1 */
    int n_hashvals, /* size of input dictionary */
    const unsigned long long *len_offsets, /* input dictionary. Part 2 */
    const char *rawdata, /* concatenated strings */
    unsigned long long ulhash, /* value for which string is to be found */
    char *buffer, /* where output is stored if found */
    int sz_buffer, 
    unsigned int *ptr_len /* length of output */
    );
