#include <stdio.h>
#include <string.h>
#define __USE_XOPEN /* Necessary for strptime to be defined */
#include <time.h>
#include <stdbool.h>
// #include "/usr/include/time.h"
#include "constants.h"
#include "macros.h"
#include "auxil.h"
// START FUNC DECL
int
date_to_I4(
    char *infile,
    char *in_format,
    char *str_out_format,
    char *outfile
    )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL, *ofp = NULL;
#define MAX_LEN_DATE 31
  char buffer[MAX_LEN_DATE+1];
  struct tm tm;
  int bufidx, valI4;
#define  ASCII 100
#define BINARY 200
  int out_format; 

  if ( ( infile == NULL ) || ( *infile == '\0' ) ) {
    ifp = stdin;
  }
  else {
    ifp = fopen(infile, "r");
    return_if_fopen_failed(ifp, infile, "r");
  }
  //-----------------------------------------------------------
  if ( ( outfile == NULL ) || ( *outfile == '\0' ) ) {
    ofp = stdout;
  }
  else {
    ofp = fopen(outfile, "w");
    return_if_fopen_failed(ifp, outfile, "w");
  }
  //-----------------------------------------------------------
  if ( ( in_format == NULL ) || ( *in_format == '\0' ) ){ go_BYE(-1); }
  if (( str_out_format == NULL ) || ( *str_out_format == '\0' )){ go_BYE(-1); }
  if ( strcasecmp(str_out_format, "ascii") == 0 ) {
    out_format = ASCII;
  }
  else if ( strcasecmp(str_out_format, "binary") == 0 ) {
    out_format = BINARY;
  }
  else {
    go_BYE(-1);
  }
  //-----------------------------------------------------------

  long long lno = 1;
  zero_string(buffer, MAX_LEN_DATE+1); bufidx = 0;
  for ( ; !feof(ifp); ) { 
    int ic = getc(ifp);
    char c = (char)ic;
    if ( ic < 0 ) { 
      if ( buffer[0] != '\0' ) { go_BYE(-1); }
    }
    if ( c == '\n' ) {  
      /* buffer full. time to convert */
      if ( bufidx == 0 ) { 
	fprintf(stderr, "Buffer empty. Error on Line %lld,  \n", lno); 
	go_BYE(-1); 
      }
      else {
        strptime(buffer, in_format, &tm);
        valI4 = mktime(&tm);
        zero_string(buffer, MAX_LEN_DATE+1); bufidx = 0;
	switch ( out_format ) { 
	  case ASCII : printf("%d\n", valI4); break;
	  case BINARY : fwrite(&valI4, sizeof(int), 1, ofp); break; 
	  default :  go_BYE(-1); break;
	}
      }
    }
    else {
      if ( bufidx >= MAX_LEN_DATE ) { 
	fprintf(stderr, "Buffer too long. Error on Line %lld,  \n", lno); 
	go_BYE(-1); 
      }
      buffer[bufidx++] = c;
    }
  }
BYE:
  if ( ( infile != NULL ) && ( *infile != '\0' ) ) {
    fclose_if_non_null(ifp);
  }
  if ( ( outfile != NULL ) && ( *outfile != '\0' ) ) {
    fclose_if_non_null(ofp);
  }
  return status ;
}
