/*
  Inputs:
  (1) A file that looks as follows. It is line-oriented. Fields are tab
  separated.

  "raw title"<tab>clean_title1,clean_title2,..clean_title_n<tab>cnt

  The output is 
  lno,hash1
  lno,hash2
  .....

  lno is line number that starts at 1. Consider a line (with lno = x)
  that has n clean  titles. Then, there will be n rows in the output
  file with lno = x.

  We can produce output in ascii or binary form. If in ascii, then it is
  one file. If it is in binary, then it is in 2 files, one for each
  column.


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
  char *infile = NULL, *prefix_outfile = NULL; 
  char *outfile = NULL, *outfile1 = NULL, *outfile2 = NULL;
  char *buf, *alt_fld = NULL; int buflen = 65536;
  char *op_file_format = NULL;
  FILE *ifp = NULL; FILE *ofp = NULL, *ofp1 = NULL, *ofp2 = NULL;
  char *cptr, *left_marker = NULL;
  int len, lno, num_op = 0;
  unsigned long long llhash;
  bool is_op_ascii = true, is_more_clean = false; 

  /* START: Underatand arguments */
  buf = (char *)malloc(buflen * sizeof(char));
  return_if_malloc_failed(buf);
  zero_string(buf, buflen);

  alt_fld = (char *)malloc(buflen * sizeof(char));
  return_if_malloc_failed(alt_fld);
  zero_string(alt_fld, buflen);

  if ( argc != 4 ) { go_BYE(-1); }

  infile = argv[1];
  if ( *infile == '\0' ) { go_BYE(-1); }
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");

  prefix_outfile = argv[2];

  op_file_format   = argv[3];

  if ( *prefix_outfile == '\0' ) {  go_BYE(-1); }

  len = strlen(prefix_outfile) + 32;

  if ( strcasecmp(op_file_format, "A") == 0 ) {

    outfile = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(outfile);
    zero_string(outfile, len);
    strcpy(outfile, prefix_outfile);
    strcat(outfile, ".txt");
    ofp = fopen(outfile, "w");
    return_if_fopen_failed(ofp, outfile, "w");

    is_op_ascii = true;
  }
  else if ( strcasecmp(op_file_format, "B") == 0 ) {

    outfile1 = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(outfile1);
    zero_string(outfile1, len);
    strcpy(outfile1, prefix_outfile);
    strcat(outfile1, ".lno.bin");
    ofp1 = fopen(outfile1, "wb");
    return_if_fopen_failed(ofp1, outfile, "wb");

    outfile2 = (char *)malloc(len * sizeof(char));
    return_if_malloc_failed(outfile2);
    zero_string(outfile2, len);
    strcpy(outfile2, prefix_outfile);
    strcat(outfile2, ".clean_thash.bin");
    ofp2 = fopen(outfile2, "wb");
    return_if_fopen_failed(ofp2, outfile, "wb");

    is_op_ascii = false;
  }
  else { go_BYE(-1); }

  if ( *infile == '\0' ) { go_BYE(-1); }
  /* STOP: Understand arguments */
  for ( lno = 1; !feof(ifp) ; lno++ ) {
    cptr = fgets(buf, buflen, ifp);
    if ( cptr == NULL ) { break; }
    if ( buf[buflen-1] != '\0' ) {
      fprintf(stderr, "Line %d was too long \n", lno+1); go_BYE(-1);
    }
    /* Advance past first tab */
    for ( cptr = buf; ( ( *cptr != '\t' ) && ( *cptr != '\0' ) ); cptr++ ) {
      if ( ( *cptr == '\0' ) || ( *cptr == '\n' ) ) { go_BYE(-1); } 
    }
    /* Now process clean titles, tab means no more */
    left_marker = ++cptr;
    is_more_clean = true;
    for ( int num_clean = 0; is_more_clean == true ; num_clean++ ) { 
      cptr = left_marker; 
      zero_string_to_nullc(alt_fld); // Buf to store clean title 
      for ( int i = 0; ; cptr++ ) { 
	if ( ( *cptr == '\0' ) || ( *cptr == '\n' ) ) { go_BYE(-1); } 
	if ( *cptr == ',' ) {
	  left_marker = ++cptr;
	  break;
	}
	else if ( *cptr == '\t' ) {
	  is_more_clean = false;
	  break;
	}
	else {
	  if ( i >= buflen ) { go_BYE(-1); }
	  alt_fld[i++] = *cptr;
	}
      }
      if ( *alt_fld != '\0' ) { 
	status = ll_hash_string(alt_fld, &llhash);
	cBYE(status);
	if ( is_op_ascii ) { 
	  fprintf(ofp, "%d,%lld\n", lno, llhash);
        }
	else {
	  fwrite(&lno, sizeof(int), 1, ofp1);
	  fwrite(&llhash, sizeof(long long), 1, ofp2);
	  num_op++;
	}
      }
      zero_string_to_nullc(alt_fld);
    }
    zero_string_to_nullc(buf);
  }
  // fprintf(stderr, "Num lines processed = %d", lno); 
  fprintf(stdout, "%d", num_op); 

  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);

  free_if_non_null(buf);
  free_if_non_null(outfile);
  free_if_non_null(outfile1);
  free_if_non_null(outfile2);
  free_if_non_null(alt_fld);

 BYE:
  return(status);
}
