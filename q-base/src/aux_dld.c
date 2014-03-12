#include "qtypes.h"
#include "auxil.h"
#include "dbauxil.h"
#include "aux_meta.h"
#include "extract_S.h"

#define MAX_LEN 32

// START FUNC DECL
int
process_aux_info(
		 char *in_aux_info, 
		 bool *ptr_ignore_hdr, 
		 char *ptr_fld_sep, 
		 char *ptr_fld_delim
		 )
// STOP FUNC DECL
{
  int status = 0;
  char fld_sep =',', fld_delim = '"'; bool ignore_hdr = false;
  bool is_null; char aux_info[MAX_LEN]; zero_string(aux_info, MAX_LEN);

  if ( ( in_aux_info != NULL ) && ( *in_aux_info != '\0' ) ) {
    status = chk_aux_info(in_aux_info); cBYE(status);
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
      fld_sep = ',';
    }
    else {
      if ( strcasecmp(aux_info, "comma") == 0 ) { 
	fld_sep = ',';
      }
      else if ( strcasecmp(aux_info, "tab") == 0 ) { 
	fld_sep = '\t';
	fld_delim = '\0'; // IMPORTANT!!!!!!
      }
      else if ( strcasecmp(aux_info, "ctrl_z") == 0 ) { 
	fld_sep = 26; /* TODO P1 NEEDS TO BE DEBUGGED */
	fld_delim = '\0'; // IMPORTANT!!!!!!
      }
      else {
	go_BYE(-1);
      }
    }
  }
  *ptr_ignore_hdr = ignore_hdr;
  *ptr_fld_sep    = fld_sep;
  *ptr_fld_delim  = fld_delim;
 BYE:
  return status ;
}

