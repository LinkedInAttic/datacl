#include "qtypes.h"
#include "auxil.h"
#include "mmap.h"
#include "aux_meta.h"
#include "extract_I4.h"
#include "extract_S.h"
#include "read_meta_data.h"

/* Given a file of meta data, read it into a an array of records */
#define MAXLINE 256
#define bad_line(fld_idx) {				\
    fprintf(stderr, "Error on Line %d \n", fld_idx+1);	\
  }

// last review 9/5/2013
// START FUNC DECL
int
read_meta_data(
	       char *infile,
	       char flds[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_FLD_NAME+1],
	       FLD_TYPE fldtype[MAX_NUM_FLDS_TO_LOAD],
	       FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS_TO_LOAD],
	       char dicts[MAX_NUM_FLDS_TO_LOAD][MAX_LEN_TBL_NAME+1],
	       int *ptr_n_flds
	       )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL;
  char *line = NULL, *bak_line = NULL;
  int n_flds = 0; 
  char  *cptr = NULL; 
#define MAX_LEN 32
  char aux_info[MAX_LEN]; bool is_null;
  /*-----------------------------------------------------------*/
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  /*-----------------------------------------------------------*/
  zero_string(aux_info, MAX_LEN);
  line = (char*)malloc(MAXLINE * sizeof(char));
  return_if_malloc_failed(line);
  /*-----------------------------------------------------------*/
  /* Count number of lines */
  status = num_lines(infile, &n_flds); cBYE(status);
  if ( n_flds == 0 ) { go_BYE(-1); }
  /*-----------------------------------------------------------*/
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( int fld_idx = 0; fld_idx < n_flds; fld_idx++ ) { 
    int num_commas = 0;
    zero_string(line, MAXLINE);
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    if ( cptr[MAXLINE-1] != '\0' ) { bad_line(fld_idx); go_BYE(-1); }
    status = count_char(cptr, ',', &num_commas); cBYE(status);
    if ( num_commas != 2 ) { bad_line(fld_idx); go_BYE(-1); }
    bak_line = line;

    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { 
      bad_line(fld_idx); go_BYE(-1); 
    }
    if ( strlen(cptr) > MAX_LEN_FLD_NAME ) { bad_line(fld_idx); go_BYE(-1); }
    strcpy(flds[fld_idx], cptr);
    status = chk_fld_name(cptr); cBYE(status);
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { 
      bad_line(fld_idx); go_BYE(-1); 
    }
    if ( strcmp(cptr, "LKP") == 0 ) { 
      fld_props[fld_idx].is_lkp = true;
      fldtype[fld_idx] = I4; 
    }
    else {
      status = unstr_fldtype(cptr, &(fldtype[fld_idx])); cBYE(status);
      if ( fldtype[fld_idx] == B  ) { bad_line(fld_idx); go_BYE(-1); }
      fld_props[fld_idx].is_lkp = false;
    }
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr != NULL ) && ( *cptr != '\0' ) ) { 
      // we have some auxiliary info to process
      /*-----------------------------------------------------------*/
      int maxlen;
      status = extract_I4(cptr, "maxlen=[", "]", &maxlen, &is_null); cBYE(status);
      if ( is_null == false ) { 
	if ( maxlen <= 0 ) { go_BYE(-1); }
	fld_props[fld_idx].maxlen = maxlen;
      }
      else {
	fld_props[fld_idx].maxlen = 0;
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_load=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	fld_props[fld_idx].is_load = true;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  fld_props[fld_idx].is_load = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  fld_props[fld_idx].is_load = false;
	}
	else {
	  bad_line(fld_idx); go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_all_def=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	fld_props[fld_idx].is_all_def = false;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  fld_props[fld_idx].is_all_def = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  fld_props[fld_idx].is_all_def = false;
	}
	else {
	  bad_line(fld_idx); go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_trunc=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	fld_props[fld_idx].is_trunc = false;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  fld_props[fld_idx].is_trunc = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  fld_props[fld_idx].is_trunc = false;
	}
	else {
	  bad_line(fld_idx); go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_dict_old=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	fld_props[fld_idx].is_dict_old = false;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  fld_props[fld_idx].is_dict_old = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  fld_props[fld_idx].is_dict_old = false;
	}
	else {
	  bad_line(fld_idx); go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_null_if_missing=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	fld_props[fld_idx].is_null_if_missing = false;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  fld_props[fld_idx].is_null_if_missing = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  fld_props[fld_idx].is_null_if_missing = false;
	}
	else {
	  bad_line(fld_idx); go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "dict=[", "]", aux_info, MAX_LEN, &is_null);
      if ( !is_null ) { 
	// TODO P1 Do not over use aux_info. Use meaningful names
	status = chk_tbl_name(aux_info); cBYE(status);
	strcpy(dicts[fld_idx], aux_info);
      }
    }
    if ( ( fld_props[fld_idx].is_lkp  == true ) && 
	 ( strlen(dicts[fld_idx]) == 0 ) ) { bad_line(fld_idx); go_BYE(-1); }
    /*-----------------------------------------------------------*/
    zero_string_to_nullc(line);
  }
  /* Check no duplicate names */
  for ( int i = 0; i < n_flds; i++ ) { 
    for ( int j = i+1; j < n_flds; j++ ) { 
      if ( strcmp(flds[i], flds[j]) == 0 ) { 
        fprintf(stderr, "ERROR. Duplicate field name %s \n", flds[i]);
	go_BYE(-1);
      }
      if ( ( strcmp(dicts[i], dicts[j]) == 0 )  && 
           ( strlen(dicts[i]) > 0 ) &&
	   ( fld_props[i].is_dict_old == false ) && 
	   ( fld_props[j].is_dict_old == false ) ) {
	go_BYE(-1);
      }
    }
  }
  /* Some other checks */
  for ( int i = 0; i < n_flds; i++ ) {
    if ( fld_props[i].is_load == false ) { 
      if ( strlen(dicts[i]) > 0 ) { go_BYE(-1); }
    }
    if ( fld_props[i].is_lkp == true ) { 
      if ( fldtype[i] != I4 ) { go_BYE(-1); }
      if ( strlen(dicts[i]) == 0 ) { go_BYE(-1); }
    }
    else {
      switch ( fldtype[i] ) { 
	case I1 : case I2 : case I4 : case I8 : case F4 : case F8 : 
	case SC : case SV : 
	break;
      default : 
	go_BYE(-1); 
	break;
	if ( strlen(dicts[i]) != 0 ) { go_BYE(-1); }
      }
      if ( fld_props[i].is_null_if_missing == true ) {
	if ( fld_props[i].is_dict_old != true ) {
	  go_BYE(-1);
	}
      }
    }
  }
  //--- Cannot have same new dict for 2 fields in same table during DLD
  for ( int i = 0; i < n_flds; i++ ) {
    if ( strlen(dicts[i]) == 0 ) { continue; }
    for ( int j = i+1; j < n_flds; j++ ) {
      if ( strlen(dicts[j]) == 0 ) { continue; }
      if ( ( fld_props[i].is_dict_old == true ) || 
	   ( fld_props[j].is_dict_old == true ) ) {
	/* Nothing to check */
      }
      else {
	if ( strcmp(dicts[i], dicts[j]) == 0 ) { 
	  fprintf(stderr, "Dictionary [%s] same for rows %d and %d \n",
		  dicts[i], i, j);
	  go_BYE(-1); }
      }
    }
  }
  //---------------------------------------

 BYE:
  free_if_non_null(line);
  fclose_if_non_null(ifp);
  *ptr_n_flds = n_flds;
  return(status);
}
