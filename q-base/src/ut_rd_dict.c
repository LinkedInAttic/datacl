#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "qtypes.h"
#include "auxil.h"
#include "mix.h"
#include "mmap.h"
#include "rd_dict.h"

/* DO NOT AUTO GENERATE HEADER FILE  */
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  FILE *ifp = NULL;
  char datafile[1024]; // TODO P2
  char keysfile[1024]; // TODO P2
  char lenoffsetfile[1024]; // TODO P2
#define MAXLINE 64
  char line[MAXLINE];
  char *buffer = NULL; int buflen = 8192;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  char *X3 = NULL; size_t nX3 = 0;
  char *rawdata = NULL;
  unsigned long long *hashvals = NULL;
  unsigned long long *len_offsets = NULL;

  buffer = malloc(buflen * sizeof(char));
  zero_string(buffer, buflen);

  if ( argc != 3 ) { go_BYE(-1); }
  char *infile = argv[1];
  char *outprefix = argv[2];

  strcpy(datafile, outprefix); strcat(datafile, ".rawdata");
  status = rs_mmap(datafile, &X1, &nX1, 0); cBYE(status);
  if ( nX1 == 0 ) {  go_BYE(-1); }
  rawdata = (char *)X1;

  strcpy(keysfile, outprefix); strcat(keysfile, ".keys");
  status = rs_mmap(keysfile, &X2, &nX2, 0); cBYE(status);
  if ( nX2 == 0 ) {  go_BYE(-1); }
  hashvals = (unsigned long long *)X2;
  int n_hashvals = (nX2 / sizeof(unsigned long long));
  if ( ( n_hashvals * sizeof(unsigned long long) ) != nX2 ) { go_BYE(-1); }

  strcpy(lenoffsetfile, outprefix); strcat(lenoffsetfile, ".lenoff");
  status = rs_mmap(lenoffsetfile, &X3, &nX3, 0); cBYE(status);
  if ( nX3 == 0 ) {  go_BYE(-1); }
  len_offsets = (unsigned long long *)X3;

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( ; !feof(ifp); ) { 
    zero_string_to_nullc(line);
    char *cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    line[strlen(line)-1] = '\0'; // delete eoln 
    char *endptr;
    unsigned int len;
    unsigned long long ulhash = strtoull(line, &endptr, 10);
    status = rd_dict(hashvals, n_hashvals, len_offsets, rawdata, ulhash, buffer, buflen, &len);
    if ( status < 0 ) { 
      fprintf(stderr, "No map for %s\n", line); 
      if ( len > 0 ) { go_BYE(-1); }
    }
    else {
      if ( len == 0 ) { go_BYE(-1); }
      fprintf(stderr, "Mapped %s to %s \n", line, buffer);
    }
  }
BYE:
  fclose_if_non_null(ifp);
  return(status);
}

