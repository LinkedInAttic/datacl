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
Output(
       int **src_bufs,      
       int *ptr_weight, 
       int *last_packet,
       int last_packet_incomplete,
       long long last_packet_siz, 
       long long siz, 
       int num_quantiles,
       int *ptr_y,
       int b,         
       long long k
       )
;
//----------------------------
