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
udf_deg2(
    char *TM_tbl,  /* member table */
    char *TM_TC_lb, /* I8 */
    char *TM_TC_cnt, /* I4 */
    char *TD1_tbl, /* first degree connections */
    char *TD1_fk_TM, /* I4 */
    char *TC_tbl, /* connections */
    char *TC_mid, /* I4 */
    char *TD2_tbl, /* second  degree connections */
    char *TD2_mid
    )
;
//----------------------------
