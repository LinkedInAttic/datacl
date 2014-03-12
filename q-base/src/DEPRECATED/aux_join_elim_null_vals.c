#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "dbauxil.h"
#include "get_nR.h"
#include "f_to_s.h"
#include "mk_file.h"
#include "aux_join.h"
#include "del_fld.h"
#include "is_nn_fld.h"
#include "is_idx_fld.h"
#include "add_tbl.h"
#include "del_tbl.h"
#include "copy_fld.h"
#include "s_to_f.h"
#include "set_idx_fld.h"
#include "get_name.h"
#include "binld.h"
#include "bindmp.h"
#include "sortbindmp.h"
#include "open_temp_file.h"
#include "aux_fld_meta.h"
#include "fld_meta.h"

int
elim_null_vals(
	char *docroot,
	sqlite3 *db,
	char *intbl,
	char *infld,
	char **ptr_out_tbl,
	long long *ptr_out_nR,
	char **ptr_idx_fld
)
{
  int status = 0;
  char *out_tbl = NULL, *idx_fld = NULL, *temp_intbl = NULL, *nn_fld = NULL;
  int len, itemp, nn_fld_id, idx_fld_id;
  char rslt_buf[1024];
  char *endptr = NULL;
  long long out_nR;

  zero_string(rslt_buf, 1024);
  status = is_nn_fld(docroot, db, intbl, infld, &nn_fld_id, rslt_buf);
  cBYE(status);
  status = is_idx_fld(docroot, db, intbl, infld, &idx_fld_id, rslt_buf);
  cBYE(status);
  if ( nn_fld_id > 0 ) { /* There are null values */
    len = strlen(intbl) + 6;
    mcr_alloc_null_str(out_tbl, len);
    strcpy(temp_intbl, "_temp_");
    strcat(temp_intbl, intbl);

    len = strlen(infld) + 6;
    mcr_alloc_null_str(nn_fld, len);
    strcpy(nn_fld, "_nn_");
    strcat(nn_fld, infld);

    zero_string(rslt_buf, 1024);
    status = f_to_s(docroot, db, intbl, nn_fld, "sum", rslt_buf);
    cBYE(status);
    out_nR = strtoll(rslt_buf, &endptr, 10);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( out_nR == 0 ) { 
      fprintf(stderr, "No non-null values in src\n"); go_BYE(-1);
    }
    status = del_tbl(docroot, db, temp_intbl);
    cBYE(status);
    status = add_tbl(docroot, db, temp_intbl, rslt_buf, &itemp);
    cBYE(status);
    status = copy_fld(docroot, db, intbl, infld, nn_fld, temp_intbl);
    cBYE(status);
    zero_string(rslt_buf, 1024);
    status = get_name(docroot, db, idx_fld_id, "fld", rslt_buf);
    cBYE(status);
    if ( idx_fld_id > 0 ) { 
      status = copy_fld(docroot, db, intbl, rslt_buf, nn_fld, temp_intbl);
      cBYE(status);
    } 
    else {
      if ( out_nR >= INT_MAX ) { 
        status = s_to_f(docroot, db, temp_intbl, "id", 
	"op=seq:start=0:incr=1:fldtype=long long");
      }
      else {
        status = s_to_f(docroot, db, temp_intbl, "id", 
	"op=seq:start=0:incr=1:fldtype=int");
      }
      status = set_idx_fld(docroot, db, temp_intbl, "id");
      cBYE(status);
    }
    len = strlen(temp_intbl) + 1;
    mcr_alloc_null_str(out_tbl, len);
    strcpy(out_tbl, temp_intbl);
  }
  else {
    if ( idx_fld_id <= 0 ) { 
      if ( out_nR >= INT_MAX ) { 
        status = s_to_f(docroot, db, intbl, "id", 
	"op=seq:start=0:incr=1:fldtype=long long");
      }
      else {
        status = s_to_f(docroot, db, intbl, "id", 
	"op=seq:start=0:incr=1:fldtype=int");
      }
      status = set_idx_fld(docroot, db, intbl, "id");
      cBYE(status);
      status = is_idx_fld(docroot, db, intbl, infld, &idx_fld_id, rslt_buf);
      cBYE(status);
    }
    len = strlen(intbl) + 1;
    mcr_alloc_null_str(out_tbl, len);
    strcpy(out_tbl, intbl);
  }
  *ptr_out_nR = out_nR;
  *ptr_out_tbl = out_tbl;
  *ptr_idx_fld = idx_fld;
BYE:
  free_if_non_null(temp_intbl);
  return(status);
}


int
aux_join_get_vals(
	  char *docroot,
	  sqlite3 *in_db,
	  char *x_tbl,
	  char *x_lnk, /* link field */
	  char *x_idx, /* idx field */
	  char *tbl,
	  char *val, /* value to be joined */
	  FLD_META_TYPE *ptr_lnk_meta,
	  FLD_META_TYPE *ptr_val_meta,
	  FLD_META_TYPE *ptr_idx_meta,
	  long long *ptr_x_nR
	  )
// STOP FUNC DECL
{
  int status = 0;
  sqlite3 *db = NULL;

  if ( ( x_tbl == NULL ) || ( *x_tbl == '\0' ) ) { go_BYE(-1); }
  if ( ( x_lnk == NULL ) || ( *x_lnk == '\0' ) ) { go_BYE(-1); }
  if ( ( x_idx == NULL ) || ( *x_idx == '\0' ) ) { go_BYE(-1); }
  if ( ( tbl == NULL ) || ( *tbl == '\0' ) ) { go_BYE(-1); }
  /* value field is NULL when we use this for dst table 
  if ( ( val == NULL ) || ( *val == '\0' ) ) { go_BYE(-1); }
  */
  status = open_db_if_needed(docroot, in_db, &db);
  cBYE(status);
  zero_fld_meta(ptr_lnk_meta);
  zero_fld_meta(ptr_idx_meta);
  if ( ( val != NULL ) && ( *val == '\0' ) ) { 
    zero_fld_meta(ptr_val_meta);
    status = fld_meta(docroot, db, tbl, val, -1, ptr_val_meta);
    cBYE(status);
  }
  status = fld_meta(docroot, db, x_tbl, x_idx, -1, ptr_idx_meta);
  cBYE(status);
  status = fld_meta(docroot, db, x_tbl, x_lnk, -1, ptr_lnk_meta);
  cBYE(status);
 BYE:
  if ( in_db == NULL ) { sqlite3_close(db); }
  return(status);
}

int
sort_if_necessary(
	char *docroot, 
	sqlite3 *db, 
	char *tbl, 
	char *fld1,
	char *fld2
)
{
  int status = 0;
  FILE *fp = NULL; char *tempfile = NULL;
  char *flds = NULL; int len = 0;

  len = strlen(fld1) + strlen(fld2) + 8;
  mcr_alloc_null_str(flds, len);
  strcpy(flds, fld1);
  strcat(flds, ":");
  strcat(flds, fld2);
  status = open_temp_file(&fp, &tempfile);
  cBYE(status);
  fclose_if_non_null(fp);
  status = bindmp(docroot, db, tbl, flds, "", tempfile);
  cBYE(status);
  status = sortbindmp(tempfile, "II", "AA");
  cBYE(status);
  status = binld(docroot, db, tbl, flds, "I:I", tempfile);
  cBYE(status);

BYE:
  return(status);
}
