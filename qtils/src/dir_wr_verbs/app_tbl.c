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
#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "tbl_meta.h"
#include "app_tbl.h"
#include "app_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "meta_globals.h"

//---------------------------------------------------------------
// Appends table T2 to table T1
// START FUNC DECL
int 
app_tbl(
       char *t1,
       char *t2
       )
// STOP FUNC DECL
{
  int status = 0;
  TBL_REC_TYPE t1_rec, t2_rec;
  int num_flds_1 = 0, num_flds_2 = 0; 
  long long nR1, nR2; int t1_id, t2_id;
  char str_new_nR[32];
  int flds_1[MAX_NUM_FLDS], flds_2[MAX_NUM_FLDS];
  //----------------------------------------------------------------
  zero_string(str_new_nR, 32);
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  for ( int i = 0; i < MAX_NUM_FLDS; i++ ) {
    flds_1[i] = INT_MIN;
    flds_2[i] = INT_MIN;
  }
  /* Check if tables exist */
  status = is_tbl(t1, &t1_id, &t1_rec); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  status = is_tbl(t2, &t2_id, &t2_rec); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  // Get primary fields in Table t1 
  status = tbl_meta(t1, false, flds_1, &num_flds_1); cBYE(status);
  status = tbl_meta(t2, false, flds_2, &num_flds_2); cBYE(status);
  if ( num_flds_1 != num_flds_2 ) { 
    fprintf(stderr, "Table %s has %d fields. Table %s has %d fields\n",
	t1, num_flds_1, t2, num_flds_2);
    go_BYE(-1);
  }
  //------------------------------------------------
  // Check that all fields in Table T1 are present in T2
  for ( int i = 0; i < num_flds_1; i++ ) { 
    bool is_present = false;
    int f1_id = flds_1[i];
    chk_range(f1_id, 0, g_n_fld);
    char *f1 = NULL, *f2 = NULL;
    f1 = g_flds[f1_id].name;
    for ( int j = 0; j < num_flds_2; j++ ) { 
      int f2_id = flds_2[j];
      chk_range(f2_id, 0, g_n_fld);
      f2 = g_flds[f2_id].name;
      if ( strcmp(f1, f2) == 0 ) {
	is_present = true;
	break;
      }
    }
    if ( !is_present ) {
      fprintf(stderr, "Field %s in Table %s not present in Table %s \n",
	  f1, t1, t2);
      go_BYE(-1);
    }
  }
  //------------------------------------------------
  /* Convince yourself that there is no need to check that all fields in
   * T2 are not in T1 */
  for ( int i = 0; i < num_flds_1; i++ ) { 
    int f1_id = flds_1[i];
    char *f1 = g_flds[f1_id].name;
    status = app_fld(t1, f1, t2, f1); cBYE(status);
  }
  //--------------------------------------------------------
  // Update nR for T1
  nR1 = g_tbls[t1_id].nR;
  nR2 = g_tbls[t2_id].nR;
  g_tbls[t1_id].nR = nR1 + nR2;
  //--------------------------------------------------------
BYE:
  return(status);
}
