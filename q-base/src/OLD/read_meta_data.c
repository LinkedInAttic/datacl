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
	       bool is_load[MAX_NUM_FLDS],
	       bool is_all_def[MAX_NUM_FLDS],
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
  for ( n_flds = 0; ; n_flds++ ) { 
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    bak_line = line;

    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    if ( strlen(cptr) > MAX_LEN_FLD_NAME ) { go_BYE(-1); }
    strcpy(flds[n_flds], cptr);
    status = chk_fld_name(cptr, 0); /* 0 => primary field */
    cBYE(status);
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr == NULL ) || ( *cptr == '\0' ) ) { go_BYE(-1); }
    status = unstr_fldtype(cptr, &(fldtype[n_flds]));
    /*-----------------------------------------------------------*/
    cptr = strsep(&bak_line, ",");
    if ( ( cptr != NULL ) && ( *cptr != '\0' ) ) { 
      // we have some auxiliary info to process
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_load=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	is_load[n_flds] = true;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  is_load[n_flds] = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  is_load[n_flds] = false;
	}
	else {
	  go_BYE(-1); 
	}
      }
      /*-----------------------------------------------------------*/
      zero_string(aux_info, MAX_LEN);
      status = extract_S(cptr, "is_all_def=[", "]", aux_info, MAX_LEN, &is_null);
      if ( is_null ) { 
	is_all_def[n_flds] = true;
      }
      else {
	if ( strcasecmp(aux_info, "true") == 0 ) { 
	  is_all_def[n_flds] = true;
	}
	else if ( strcasecmp(aux_info, "false") == 0 ) { 
	  is_all_def[n_flds] = false;
	}
	else {
	  go_BYE(-1); 
	}
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
