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
  (1) "tolower:isalnum" if we wish to do both or 
  (2) "" if we wish to do nothing

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
  char *infile = NULL, *uq_hash_file = NULL, *pref_outfile = NULL;
  char *str_fld_num = NULL; char *endptr = NULL;
  char *out1 = NULL, *out2 = NULL, *out3 = NULL, *out4 = NULL;
  char *buf, *alt_fld = NULL; int buflen = 65536;
  char *op_file_format = NULL;
  FILE *ifp = NULL; FILE *ofp = NULL; 
  FILE *ofp1 = NULL, *ofp2 = NULL, *ofp3 = NULL, *ofp4 = NULL;
  char *cptr, *fld = NULL;
  int i, len, idx, lno, num_titles = 0, desired_fld_num;
  long long *uq_hash = NULL;  int n_hash = 0; 
  long long lhash; unsigned long long ulhash;
  char *X = NULL; size_t nX = 0;
  bool *is_printed = NULL; bool to_pr, is_ascii;
  unsigned char c_nn;

  /* START: Understand arguments */
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

  uq_hash_file = argv[2];
  if ( *uq_hash_file == '\0' ) { go_BYE(-1); }
  status = rs_mmap(uq_hash_file, &X, &nX, 0);
  cBYE(status);
  n_hash = nX / sizeof(long long);
  if ( ( n_hash * sizeof(long long) ) != nX ) {
    go_BYE(-1);
  }
  uq_hash = (long long *)X;
  is_printed = (bool *)malloc(n_hash * sizeof(bool));
  return_if_malloc_failed(is_printed);
  for ( int i = 0; i < n_hash; i++ ) { 
    is_printed[i] = false;
  }
  /* Check sorted order */
  for ( int i = 1; i < n_hash; i++ ) { 
    if ( uq_hash[i] < uq_hash[i-1] ) { go_BYE(-1); }
  }

  str_fld_num = argv[3];
  if ( *str_fld_num == '\0' ) { go_BYE(-1); }
  desired_fld_num = strtol(str_fld_num, &endptr, 10);
  if ( ( *endptr != '\0' ) && ( *endptr != '\n' ) ) { go_BYE(-1); }
  if ( desired_fld_num <= 0 ) { go_BYE(1); }

  op_file_format = argv[5];
  pref_outfile = argv[4];
  if ( *pref_outfile == '\0' ) { go_BYE(-1); }
  len = strlen(pref_outfile) + 32;

  out1 = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(out1);
  zero_string(out1, len);

  out2 = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(out2);
  zero_string(out2, len);

  out3 = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(out3);
  zero_string(out3, len);

  out4 = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(out4);
  zero_string(out4, len);

  strcpy(out1, pref_outfile);
  strcpy(out2, pref_outfile);
  strcpy(out3, pref_outfile);
  strcpy(out4, pref_outfile);
  if ( strcasecmp(op_file_format, "A") == 0 ) {
    strcat(out1, ".txt");
    ofp = fopen(out1, "w");
    return_if_fopen_failed(ofp, out1, "w");
    is_ascii = true;
  }
  else {
    strcat(out1, ".hash.bin");
    ofp1 = fopen(out1, "wb");
    return_if_fopen_failed(ofp1, out1, "wb");

    strcat(out2, ".text.bin");
    ofp2 = fopen(out2, "wb");
    return_if_fopen_failed(ofp2, out2, "wb");

    strcat(out3, ".text.sz.bin");
    ofp3 = fopen(out3, "wb");
    return_if_fopen_failed(ofp3, out3, "wb");

    strcat(out4, ".text.nn.bin");
    ofp4 = fopen(out4, "wb");
    return_if_fopen_failed(ofp4, out4, "wb");

    is_ascii = false;
  }

  /* STOP: Understand arguments */

  for ( lno = 0; !feof(ifp) ; lno++ ) {
    cptr = fgets(buf, buflen, ifp);
    if ( cptr == NULL ) { break; }
    if ( buf[buflen] != '\0' ) {
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
	status = ll_hash_string(alt_fld, &ulhash);
	cBYE(status);
	lhash = (long long) ulhash;
	/* See if this has been printed */
	to_pr = false;
	idx = 0; 
	for ( idx = 0; idx < n_hash; idx++ ) {
	  if ( uq_hash[idx] == lhash ) {
	    if ( is_printed[idx] == false ) {
	      to_pr = true;
	    }
	    else {
	      to_pr = false;
	    }
	    break;
	  }
	}
	/* If so, then print out row of lookup table */
	if ( to_pr ) {
	  is_printed[idx] = true;
          num_titles++;
	  if ( is_ascii ) {
	  fprintf(ofp, "%d,%lld,\"", lno, lhash);
	  for ( cptr = alt_fld; *cptr != '\0'; cptr++ ) {
	    if ( ( *cptr == '\\' ) || ( *cptr == '"' ) ) {
	      fprintf(ofp, "\"");
	    }
	    fprintf(ofp, "%c", *cptr);
	  }
	  fprintf(ofp, "\"\n");
	  }
	  else {
	    len = strlen(alt_fld) + 1; /* +1 to print nullc */
	    if ( len > 1 ) { c_nn = TRUE; } else { c_nn = FALSE; }
	    fwrite(&lhash, sizeof(long long), 1, ofp1);
	    fwrite(&len, sizeof(int), 1, ofp3);
	    fwrite(alt_fld, sizeof(char), len, ofp2);
	    fwrite(&c_nn, sizeof(char), 1, ofp4);
	  }
	}
	zero_string_to_nullc(alt_fld);
      }
    }
    zero_string_to_nullc(buf);
  }
  // fprintf(stderr, "Processed %d lines\n", lno);
  fprintf(stdout, "%d", num_titles);

  rs_munmap(X, nX);
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  fclose_if_non_null(ofp3);
  fclose_if_non_null(ofp4);
  free_if_non_null(buf);
  free_if_non_null(alt_fld);
  free_if_non_null(is_printed);
  free_if_non_null(out1);
  free_if_non_null(out2);
  free_if_non_null(out3);

 BYE:
  return(status);
}
