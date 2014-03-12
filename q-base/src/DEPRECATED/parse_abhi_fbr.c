#include <stdio.h>
#include <string.h>
#define __USE_XOPEN /* Necessary for strptime to be defined */
#include <time.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"

#define BUFLEN 65536
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  FILE *fp = NULL;
  FILE *ofp = NULL; // output
  if ( argc != 3 ) { go_BYE(-1); }
  char *infile = argv[1];
  char *opfile = argv[2];
  char fbrbuf[BUFLEN];
  char buffer[BUFLEN];
  int tidx, bidx;
  char *needle = " in ";
  int needle_len = strlen(needle);
  struct tm tm;
  char *date_format = "%Y/%m/%d %H:%M:%S"; // 2012/09/25 00:27:26.134 
  int mid; long long ltime;

  ofp = fopen(opfile, "w"); return_if_fopen_failed(ofp, opfile, "w");

  zero_string(buffer, BUFLEN);
  zero_string(fbrbuf, BUFLEN);
  fp = fopen(infile, "r"); 
  return_if_fopen_failed(fp, infile, "r");
  for ( int lno = 1; !feof(fp); lno++) { 
    char *cptr = fgets(buffer, BUFLEN, fp);
    if ( cptr == NULL ) { break; }
    sscanf(buffer, "%d\t%lld\t%s\n", &mid, &ltime, fbrbuf);
    cptr = strtok(fbrbuf, ",");
    if ( cptr == NULL ) { go_BYE(-1); }
    fprintf(ofp,"%d\t%lld\t%s\n", mid, ltime, cptr);
    for ( ; ; ) { 
      cptr = strtok(NULL, ",");
      if ( cptr == NULL ) { break; }
      fprintf(ofp,"%d\t%lld\t%s\n", mid, ltime, cptr);
    }
    
    zero_string_to_nullc(fbrbuf);
    zero_string_to_nullc(buffer);
  }
BYE:
  fclose_if_non_null(fp);
  fclose_if_non_null(ofp);
  return(status);
}
