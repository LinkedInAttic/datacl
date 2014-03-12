/*
  Inputs:
  (1) A file that looks as follows. It is line-oriented. Fields are tab
  separated.

  MEMBER_SK	CHRON_RANK	COMPANY_NAME	USER_SUPPLIED_TITLE
  2800325	6	Danish Broadcasting Corporation	Contract Manager/Procurement Advisor
  1376	2	Ning	VP of Finance
  1376	4	Xoom	VP of Finance
  1376	5	PayPal	Senior Business Analyst

  (2) The field we wish to extract. If we wanted titles, this would be
  "4", indicating the 4th column.

  (3) Options indicating any operations we wish to perform on the field.
  Examples that are supported are
  (a) tolower -- means that we lower case values
  (b) isalnum -- means that we discard non-ascii values

  This is specified as 
  (a) "tolower:isalnum" if we wish to do both or 
  (b) "" if we wish to do nothing

  (4) Output file format -- whether A (ascii) or B (binary)

  The output is a 64-bit hash of the title, one for each row
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "hash_string.h"

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *infile = NULL, *prefix_outfile = NULL, *outfile = NULL;
  char *buf, *alt_fld = NULL; int buflen = 65536;
  char *str_fld_num = NULL;
  char *pre_proc_options = NULL; char *op_file_format = NULL;
  FILE *ifp = NULL; FILE *ofp = NULL; char *cptr, *endptr, *fld = NULL;
  int i, len, lno, num_titles = 0, desired_fld_num;
  unsigned long long lhash;
  bool is_op_ascii = true;

  /* START: Underatand arguments */
  buf = (char *)malloc(buflen * sizeof(char));
  return_if_malloc_failed(buf);
  zero_string(buf, buflen);

  alt_fld = (char *)malloc(buflen * sizeof(char));
  return_if_malloc_failed(alt_fld);
  zero_string(alt_fld, buflen);

  if ( argc != 6 ) { go_BYE(-1); }

  infile = argv[1];
  if ( *infile == '\0' ) { go_BYE(-1); }
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");

  prefix_outfile = argv[2];
  if ( *prefix_outfile == '\0' ) { go_BYE(-1); }

  str_fld_num = argv[3];
  if ( *str_fld_num == '\0' ) { go_BYE(-1); }
  desired_fld_num = strtol(str_fld_num, &endptr, 10);
  if ( ( *endptr != '\0' ) && ( *endptr != '\n' ) ) { go_BYE(-1); }
  if ( desired_fld_num <= 0 ) { go_BYE(1); }

  pre_proc_options = argv[4];
  op_file_format   = argv[5];

  len = strlen(prefix_outfile) + 32;
  outfile = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(outfile);
  zero_string(outfile, len);
  strcpy(outfile, prefix_outfile);
  if ( strcasecmp(op_file_format, "A") == 0 ) {
    strcat(outfile, ".txt");
    ofp = fopen(outfile, "w");
    return_if_fopen_failed(ofp, outfile, "w");
    is_op_ascii = true;
  }
  else if ( strcasecmp(op_file_format, "B") == 0 ) {
    strcat(outfile, ".bin");
    ofp = fopen(outfile, "wb");
    return_if_fopen_failed(ofp, outfile, "wb");
    is_op_ascii = false;
  }
  else { go_BYE(-1); }

  if ( *infile == '\0' ) { go_BYE(-1); }
  /* STOP: Underatand arguments */
  for ( lno = 0; !feof(ifp) ; lno++ ) {
    cptr = fgets(buf, buflen, ifp);
    if ( cptr == NULL ) { break; }
    if ( buf[buflen-1] != '\0' ) {
      fprintf(stderr, "Line %d was too long \n", lno+1); go_BYE(-1);
    }
    if ( lno == 0 ) { // Skip header line 
      continue; 
    }
    cptr = buf;
    for ( int fidx = 1; fidx <= 4 ; fidx++ ) {
      fld = strsep(&cptr, "\t");
      if ( fidx == desired_fld_num ) {
	for ( i = 0, cptr = fld; 
	      ( ( *cptr != '\0' ) && ( *cptr != '\n' )); cptr++ ) { 
	  if ( isspace(*cptr) ) {
	    alt_fld[i++] = ' ';
	  }
	  else if ( !isalnum(*cptr) ) {
	    if ( ( i > 0 ) && ( alt_fld[i-1] != ' ' ) ) {
	      alt_fld[i++] = ' ';
	    }
	    else {
	      continue;
	    }
	  }
	  else {
	    alt_fld[i++] = tolower(*cptr);
	  }
	}
        // fprintf(stderr, "DBG: Field [%s] \n", alt_fld);
	status = ll_hash_string(alt_fld, &lhash);
	if ( is_op_ascii ) { 
	  fprintf(ofp, "%lld\n", lhash);
	}
	else {
	  fwrite(&lhash, sizeof(unsigned long long), 1, ofp);
	}
	zero_string_to_nullc(alt_fld);
        num_titles++;
      }
    }
    
    zero_string_to_nullc(buf);
  }
  // fprintf(stderr, "Processed %d lines\n", lno);
  fprintf(stdout, "%d", num_titles);

  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  free_if_non_null(buf);
  free_if_non_null(outfile);
  free_if_non_null(alt_fld);
 BYE:
  return(status);
}
