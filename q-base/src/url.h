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
extern char 
from_hex(
    char ch
    ) 
;
//----------------------------
extern char 
to_hex(
    char code
    ) 
;
//----------------------------
extern int 
url_encode(
    char *in_str,
    char **ptr_out_str
    )
;
//----------------------------
extern int 
url_decode(
    char *in_str,
    char **ptr_out_str
    ) 
;
//----------------------------
extern int
ext_url_encode(
    char *in_str,
    char *rslt_buf,
    int sz_rslt_buf
    )
;
//----------------------------
extern int
ext_url_decode(
    char *in_str,
    char *rslt_buf,
    int sz_rslt_buf
    )
;
//----------------------------
