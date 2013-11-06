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
#include "auxil.h"
#include "mmap.h"
#include "aux_meta.h"
#include "extract_S.h"
#include "read_meta_data.h"

/* Given a file of meta data, read it into a an array of records */
#define MAXLINE 256
// START FUNC DECL
int
read_meta_data(
	       char *infile,
	       char flds[MAX_NUM_FLDS][MAX_LEN_FLD_NAME+1],
	       FLD_TYPE fldtype[MAX_NUM_FLDS],
	       FLD_PROPS_TYPE fld_props[MAX_NUM_FLDS],
	       char dicts[MAX_NUM_FLDS][MAX_LEN_FILE_NAME+1],
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
  zero_string(aux_info, MAX_LEN);
  line = (char*)malloc(MAXLINE * sizeof(char));
  return_if_malloc_failed(line);
  /*-----------------------------------------------------------*/
  /* Count number of lines */
  status = num_lines(infile, &n_flds);
  cBYE(status);
  if ( n_flds == 0 ) { go_BYE(-1); }
  /*-----------------------------------------------------------*/
  /*-----------------------------------------------------------*/
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { go_BYE(-1); }
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( int fld_idx = 0; fld_idx < n_flds; fld_idx++ ) { 
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    bak_line = line;

    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    if ( strlen(cptr) > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
    strcpy(flds[fld_idx], cptr);
    status = chk_fld_name(cptr, 0); /* 0 => primary field */
    cBYE(status);
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    status = unstr_fldtype(cptr, &(fldtype[fld_idx])); cBYE(status);
    if ( strcmp(cptr, "str") == 0 ) { 
      fld_props[fld_idx].is_string = true;
    }
    else {
      fld_props[fld_idx].is_string = false;
    }
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr != NULL ) && ( *cptr != '\0' ) ) { 
      // we have some auxiliary info to process
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
	  go_BYE(-1); 
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
	  go_BYE(-1); 
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
	  go_BYE(-1); 
	}
	fld_props[fld_idx].is_string = true;
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
	  go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "dict=[", "]", aux_info, MAX_LEN, &is_null);
      if ( !is_null ) { 
	status = chk_fld_name(aux_info, 0); cBYE(status);
	strcpy(dicts[fld_idx], aux_info);
	fld_props[fld_idx].is_string  = true;
      }
    }
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
    }
  }

 BYE:
  free_if_non_null(line);
  fclose_if_non_null(ifp);
  *ptr_n_flds = n_flds;
  return(status);
}
