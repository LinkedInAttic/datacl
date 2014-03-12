#include "qtypes.h"
#include "read_meta_data.h"
#include "mmap.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "read_csv.h"
#include "del_tbl.h"
#include "add_tbl.h"
#include "add_fld.h"
#include "add_aux_fld.h"
#include "extract_S.h"
#include "dld.h"

#define MAX_NUM_FLDS 256
#define MAX_LEN 32
// START FUNC DECL
int 
dld(
    char *tbl,
    char *meta_data_file,
    char *data_file,
    char *in_aux_info
    )
// STOP FUNC DECL
{
  int status = 0;
  int n_flds, tbl_id = -1;
  long long num_rows;
  char str_nR[32];
  char str_fld_info[4096];
  char fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1];
  char nn_fnames[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1];
  char *X = NULL; size_t nX; 
  char *cptr;
  int f1_id, nn_f1_id;

  TBL_REC_TYPE tbl_rec;
  bool is_load[MAX_NUM_FLDS];
  bool is_all_def[MAX_NUM_FLDS];
  bool is_null_null[MAX_NUM_FLDS];
  FLD_TYPE fldtype[MAX_NUM_FLDS];
  char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1];
  char aux_info[MAX_LEN]; bool is_null;
  bool ignore_hdr = false; // default 
  char fld_delim = '"'; // default 
  char fld_sep = ','; // default 

  for  ( int i = 0; i < MAX_NUM_FLDS; i++ ) {
    is_all_def[i] = false;
    is_null_null[i] = false;
    is_load[i] = true;
    zero_string(flds[i], MAX_LEN_FLD_NAME+1);
    zero_string(fnames[i], MAX_LEN_FILE_NAME+1);
    zero_string(nn_fnames[i], MAX_LEN_FILE_NAME+1);
  }
  status = read_meta_data(meta_data_file, flds, fldtype, is_load, is_all_def, &n_flds);
  cBYE(status);
  /*----------------------------------------------------*/
  if ( ( in_aux_info != NULL ) && ( *in_aux_info != '\0' ) ) {
    zero_string(aux_info, MAX_LEN);
    status = extract_S(in_aux_info, "ignore_hdr=[", "]", aux_info, MAX_LEN, &is_null);
    if ( is_null ) { 
      ignore_hdr = false;
    }
    else {
      if ( strcasecmp(aux_info, "true") == 0 ) { 
	ignore_hdr = true;
      }
      else if ( strcasecmp(aux_info, "false") == 0 ) { 
	ignore_hdr = false;
      }
      else {
	go_BYE(-1);
      }
    }
    zero_string(aux_info, MAX_LEN);
    status = extract_S(in_aux_info, "fld_sep=[", "]", aux_info, MAX_LEN, &is_null);
    if ( is_null ) { 
      ignore_hdr = false;
    }
    else {
      if ( strcasecmp(aux_info, "comma") == 0 ) { 
	fld_sep = ',';
      }
      else if ( strcasecmp(aux_info, "tab") == 0 ) { 
	fld_sep = '\t';
	fld_delim = '\0'; // IMPORTANT!!!!!!
      }
      else {
	go_BYE(-1);
      }
    }
  }

  status = read_csv(data_file, fld_delim, fld_sep, '\n', ignore_hdr, flds, n_flds, 
      fldtype, is_null_null, is_load, is_all_def, fnames, nn_fnames, &num_rows);
  cBYE(status);
  sprintf(str_nR, "%lld", num_rows);
#ifdef XXX
  status = del_tbl(tbl, -1);
  cBYE(status);
  status = add_tbl(tbl, str_nR, &tbl_id, &tbl_rec);
  cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    bool is_any_null;
    if ( strlen(flds[i].filename) == 0 ) {
      continue;
    }
    zero_string(str_fld_info, 4096);
    status = fld_meta_to_str(flds[i], str_fld_info);
    cBYE(status);
    status = add_fld(tbl, flds[i].name, str_fld_info, &f1_id);
    cBYE(status);
    zero_string_to_nullc(str_fld_info);
    /* START: Decide whether to add nn fied or not */
    if ( !is_all_def ) { 
    if ( nn_fnames[i] != NULL ) {  /* TODO: Be more specific */
      status = rs_mmap(nn_fnames[i], &X, &nX, 0);
      cBYE(status);
      is_any_null = false;
      for ( unsigned int i = 0; i < nX; i++ ) { 
	if ( X[i] == FALSE ) { 
	  is_any_null = true;
	  break;
	}
      }
      if ( is_any_null ) { 
        status = add_aux_fld(tbl, flds[i].name, nn_fnames[i], "nn",
	    &nn_f1_id);
        cBYE(status);
      }
      else { /* nn file is not needed */
	unlink(nn_fnames[i]);
      }
    }
    }
    /* STOP : Decide whether to add nn fied or not */
  }
  for ( int i = 0; i < n_flds; i++ ) { 
    free_if_non_null(nn_fnames[i]);
  }
  free_if_non_null(nn_fnames);
#endif
BYE:
  return(status);
}
