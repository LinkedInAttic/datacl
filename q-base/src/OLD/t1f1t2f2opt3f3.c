#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "fsize.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "dbauxil.h"
#include "aux_fld_meta.h"
#include "add_fld.h"
#include "is_tbl.h"
#include "is_fld.h"
#include "mk_file.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "meta_globals.h"

extern bool g_write_to_temp_dir;

//---------------------------------------------------------------
// START FUNC DECL
int 
t1f1t2f2opt3f3(
	       char *t1,
	       char *f1,
	       char *t2,
	       char *f2,
	       char *op,
	       char *t3,
	       char *f3
	       )
// STOP FUNC DECL
{
  int status = 0;
  char *f1_X = NULL; size_t f1_nX = 0;
  char *f2_X = NULL; size_t f2_nX = 0;
  char *f3_X = NULL; size_t f3_nX = 0;
  int t1_id = INT_MIN, t2_id = INT_MIN, t3_id = INT_MIN;
  int f1_id = INT_MIN, f2_id = INT_MIN, f3_id = INT_MIN;
  FLD_TYPE *f1_meta = NULL, *f2_meta = NULL;
  long long nR1 = INT_MIN, nR2 = INT_MIN; 
  long long nR3 = INT_MIN, max_nR3 = INT_MIN;

  char str_meta_data[1024];
  char *opfile = NULL; FILE *ofp = NULL;
  //----------------------------------------------------------------
  if ( ( t1 == NULL ) || ( *t1 == '\0' ) ) { go_BYE(-1); }
  if ( ( f1 == NULL ) || ( *f1 == '\0' ) ) { go_BYE(-1); }
  if ( ( t2 == NULL ) || ( *t2 == '\0' ) ) { go_BYE(-1); }
  if ( ( f2 == NULL ) || ( *f2 == '\0' ) ) { go_BYE(-1); }
  if ( ( t3 == NULL ) || ( *t3 == '\0' ) ) { go_BYE(-1); }
  if ( ( f3 == NULL ) || ( *f3 == '\0' ) ) { go_BYE(-1); }
  if ( strcmp(t1, t3) == 0 ) { go_BYE(-1); }
  if ( strcmp(t2, t3) == 0 ) { go_BYE(-1); }
  zero_string(str_meta_data, 1024);
  //--------------------------------------------------------
  status = is_tbl(t1, &t1_id); cBYE(status);
  chk_range(t1_id, 0, g_n_tbl);
  nR1 = g_tbl[t1_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, t1_id, f1, &f1_id); cBYE(status);
  chk_range(f1_id, 0, g_n_fld);
  f1_meta = &(g_fld[f1_id]);
  status = rs_mmap(f1_meta->filename, &f1_X, &f1_nX, 0); cBYE(status);
  // f1 must be sorted ascending 
  if ( g_fld[f1_id].sorttype != 1 ) { go_BYE(-1); }
  // Have not implemented case where f1 has null field 
  if ( f1_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  // Have implemented only for int 
  if ( strcmp(f1_meta->fldtype, "int") != 0 ) { cBYE(-1); } 
  //--------------------------------------------------------
  status = is_tbl(t2, &t2_id); cBYE(status);
  chk_range(t2_id, 0, g_n_tbl);
  nR2 = g_tbl[t2_id].nR;
  //--------------------------------------------------------
  status = is_fld(NULL, t2_id, f2, &f2_id); cBYE(status);
  chk_range(f2_id, 0, g_n_fld);
  f2_meta = &(g_fld[f2_id]);
  status = rs_mmap(f2_meta->filename, &f2_X, &f2_nX, 0); cBYE(status);
  // f2 must be sorted ascending 
  if ( g_fld[f2_id].sorttype != 1 ) { go_BYE(-1); }
  // Have not implemented case where f2 has null field 
  if ( f2_meta->nn_fld_id >= 0 ) { go_BYE(-1); }
  // Have implemented only for int 
  if ( strcmp(f2_meta->fldtype, "int") != 0 ) { cBYE(-1); } 
  //--------------------------------------------------------
  /* Make space for output */
  /* Note thgat nR1 + nR2 is upper bound. May need to trim this later*/

  //--------------------------------------------------------
  int *f1ptr = NULL, *f2ptr = NULL, *f3ptr = NULL;
  int f1val, f2val;
  int *endf1ptr = NULL, *endf2ptr = NULL;
  //--------------------------------------------------------
  if ( strcmp(op, "intersection") == 0 ) {
    max_nR3 = max(nR1 , nR2);
    status = open_temp_file(&ofp, &opfile, max_nR3*sizeof(int)); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(opfile, max_nR3*sizeof(int)); cBYE(status);
    status = rs_mmap(opfile, &f3_X, &f3_nX, 1); cBYE(status);
    nR3 = 0;
    f1ptr = (int *)f1_X; endf1ptr = f1ptr + nR1;
    f2ptr = (int *)f2_X; endf2ptr = f2ptr + nR2;
    f3ptr = (int *)f3_X; 
    for ( ; ; ) {
      if ( f1ptr == endf1ptr ) { break; }
      if ( f2ptr == endf2ptr ) { break; }
      f1val = *f1ptr;
      f2val = *f2ptr;
      if ( f1val == f2val ) { 
	*f3ptr = f1val;
	f3ptr++;
	nR3++;
	if ( nR3 > (nR1+nR2) ) { go_BYE(-1); }
	f1ptr++;
	f2ptr++;
      }
      else if ( f1val < f2val ) { 
	f1ptr++;
      }
      else if ( f2val < f1val ) { 
	f2ptr++;
      }
    }
  }
  else if ( strcmp(op, "union") == 0 ) {
    max_nR3 = nR1 + nR2;
    status = open_temp_file(&ofp, &opfile, max_nR3*sizeof(int)); cBYE(status);
    fclose_if_non_null(ofp);
    status = mk_file(opfile, max_nR3*sizeof(int)); cBYE(status);
    status = rs_mmap(opfile, &f3_X, &f3_nX, 1); cBYE(status);

    nR3 = 0;
    f1ptr = (int *)f1_X; endf1ptr = f1ptr + nR1;
    f2ptr = (int *)f2_X; endf2ptr = f2ptr + nR2;
    f3ptr = (int *)f3_X; 
    for ( ; ( ( f1ptr != endf1ptr ) || ( f2ptr != endf2ptr ) ); ) { 
      if ( f1ptr == endf1ptr ) { /* copy items from f2 */
        f2val = *f2ptr;
	*f3ptr = f2val;
	f3ptr++;
	f2ptr++;
	nR3++;
      }
      else if ( f2ptr == endf2ptr ) { /* copy items from f1 */
        f1val = *f1ptr;
	*f3ptr = f1val;
	f3ptr++;
	f1ptr++;
	nR3++;
      }
      else {
	f1val = *f1ptr;
	f2val = *f2ptr;
	if ( f1val < f2val ) { 
	  *f3ptr = f1val;
	  f1ptr++;
	}
	else if ( f1val == f2val ) { 
	  *f3ptr = f1val;
	  f1ptr++;
	  f2ptr++;
	}
	else {
	  *f3ptr = f2val;
	  f2ptr++;
	}
        f3ptr++;
	nR3++;
      }
    }
  }
  else { go_BYE(-1); }
  rs_munmap(f3_X, f3_nX);
  truncate(opfile, (sizeof(int) * nR3));

  // Delete dst_tbl if it exists
  status = del_tbl(t3, -1); cBYE(status);
  char str_nR3[32]; sprintf(str_nR3, "%lld", nR3);
  status = add_tbl(t3, str_nR3, &t3_id); cBYE(status);
  // Add count field to meta data 
  sprintf(str_meta_data, "fldtype=int:n_sizeof=4:filename=%s", opfile);
  status = add_fld(t3, f3, str_meta_data, &f3_id); cBYE(status);
  // Since f1 and f2 are sorted ascending, so musyt f3 be 
  chk_range(f3_id, 0, g_n_fld);
  g_fld[f3_id].sorttype = 1; // sorted ascending 
  //-----------------------------------------------------------
 BYE:
  g_write_to_temp_dir = false;
  rs_munmap(f1_X, f1_nX);
  rs_munmap(f2_X, f2_nX);
  rs_munmap(f3_X, f3_nX);
  free_if_non_null(opfile);
  return(status);
}
