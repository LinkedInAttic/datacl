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
  FILE *tfp = NULL; // timestamp 
  FILE *rfp = NULL; // response time 
  if ( argc != 3 ) { go_BYE(-1); }
  char *infile = argv[1];
  char *prefix = argv[2];
  char timestamp[BUFLEN];
  char resp_time[BUFLEN];
  char buffer[BUFLEN];
  int tidx, bidx;
  char *needle = " in ";
  int needle_len = strlen(needle);
  struct tm tm;
  char *date_format = "%Y/%m/%d %H:%M:%S"; // 2012/09/25 00:27:26.134 

  strcpy(buffer, prefix); strcat(buffer, "_time.bin");
  tfp = fopen(buffer, "wb"); return_if_fopen_failed(tfp, buffer, "wb");

  strcpy(buffer, prefix); strcat(buffer, "_resp.bin");
  rfp = fopen(buffer, "wb"); return_if_fopen_failed(rfp, buffer, "wb");

  zero_string(resp_time, BUFLEN);
  zero_string(timestamp, BUFLEN);
  zero_string(buffer, BUFLEN);
  fp = fopen(infile, "r"); 
  return_if_fopen_failed(fp, infile, "r");
  int skip_count = 0;
  for ( int lno = 1; !feof(fp); lno++) { 
    char *cptr = fgets(buffer, BUFLEN, fp);
    if ( cptr == NULL ) { break; }
    // read timestamp
    bidx = 0; tidx = 0;
    cptr = buffer;
    for ( tidx = 0; tidx < BUFLEN; tidx++ ) { 
      if ( *cptr == '[' ) { break; }
      if ( *cptr == '.' ) { break; } // we throw away the msec portion 
      timestamp[tidx] = *cptr++;
    }
    if ( timestamp[tidx] == ' ' ) { timestamp[tidx] = '\0'; }
    strptime(timestamp, date_format, &tm);
    int itime = mktime(&tm);
    //-- read response time 
    cptr = strstr(buffer, needle); 
    if ( cptr == NULL ) {  // there was a failure 
      // TODO: Keep failures in separate table
      skip_count++;
      continue;
    }
    cptr += needle_len;
    for ( int ridx = 0; ridx < BUFLEN ; ridx++ ) { 
      if ( ( ( *cptr < '0' ) || ( *cptr > '9' ) ) && ( *cptr != '.' ) ) { break; }
      resp_time[ridx] = *cptr++;
    }
    //----------
    // Do conversions before printing 
    char *endptr;
    float ftime = strtod(resp_time, &endptr);
    if ( *endptr != '\0' ) { go_BYE(-1); }
    int iresp = (int)(ftime * 1000.0); // convert to msec
    //----------
    // fprintf(stderr, "%s--%s--%d--%d\n", timestamp, resp_time, itime, iresp);
    fwrite(&itime, sizeof(int), 1, tfp);
    fwrite(&iresp, sizeof(int), 1, rfp);
    
    zero_string_to_nullc(timestamp);
    zero_string_to_nullc(resp_time);
    zero_string_to_nullc(buffer);
  }
  fprintf(stderr, "Skipped %d lines \n", skip_count);
BYE:
  fclose_if_non_null(fp);
  fclose_if_non_null(rfp);
  fclose_if_non_null(tfp);
  return(status);
}
