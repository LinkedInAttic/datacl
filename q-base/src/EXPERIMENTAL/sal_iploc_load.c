#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"

#define NUMBUFS 11
#define MAXLINE 8192

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile = NULL;
  char line[MAXLINE];
  FILE *ifp = NULL; 
  char *cptr = NULL, *endptr = NULL;
  char bufs[NUMBUFS][MAXLINE];
  bool skip_this_line = false;
  int mid;  float flat, flon;
  FILE *latfp, *lonfp, *midfp = NULL;
  int num_lines_skipped = 0, num_lines_written = 0;

  for ( int i = 0; i < NUMBUFS; i++ ) { 
    zero_string(bufs[i], MAXLINE);
  }
  latfp = fopen("_tempf_latfile", "wb");
  return_if_fopen_failed(latfp, "_tempf_latfile", "wb");
  lonfp = fopen("_tempf_lonfile", "wb");
  return_if_fopen_failed(lonfp, "_tempf_lonfile", "wb");
  midfp = fopen("_tempf_midfile", "wb");
  return_if_fopen_failed(midfp, "_tempf_midfile", "wb");

  zero_string(line, MAXLINE);
  if ( argc != 2 ) { go_BYE(-1); }
  infile = argv[1];
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( ; !feof(ifp); ) { 
    skip_this_line = false;
    fgets(line, MAXLINE, ifp);
    if ( *line == '\0' ) { break; }
    cptr = line;
    // Split
    for ( int j = 0; j < NUMBUFS; j++ ) { 
      for ( int i = 0; ( ( *cptr != '\t') && ( *cptr != '\n' ) ) ; cptr++ ) { 
        bufs[j][i++] = *cptr;
      }
      cptr++;
      // fprintf(stderr, "bufs[%d] = %s \n", j, bufs[j]);
      switch ( j ) { 
	case 0 : 
	  break;
	case 1 : 
	case 2 : 
	case 3 : 
	  if ( ( bufs[j][0] == '\0' ) || ( strcmp(bufs[j], "\"\"") == 0) ) {
	    skip_this_line = true;
	  }
	  break;
	case 4 : 
	  break;
	case 5 : 
	  flat = strtof(bufs[j], &endptr);
	  if ( *endptr != '\0' ) { go_BYE(-1); }
	  break;
	case 6 : 
	  flon = strtof(bufs[j], &endptr);
	  if ( *endptr != '\0' ) { go_BYE(-1); }
	  break;
	case 7 : 
	case 8 : 
	  break;
	case 9 : 
	  mid = strtol(bufs[j], &endptr, 10);
	  if ( *endptr != '\0' ) { go_BYE(-1); }
	case 10 : 
	case 11 : 
	  break;
	default : 
	  go_BYE(-1);
	  break;
      }
      zero_string_to_nullc(bufs[j]);
    }
    if ( !skip_this_line ) { 
      num_lines_written++;
      fwrite(&flat, sizeof(float), 1, latfp);
      fwrite(&flon, sizeof(float), 1, lonfp);
      fwrite(&mid, sizeof(int),   1, midfp);
    }
    else {
      num_lines_skipped++;
    }
    zero_string_to_nullc(line);
  }
  fprintf(stderr, "num_lines_skipped = %d \n", num_lines_skipped);
  fprintf(stderr, "num_lines_written = %d \n", num_lines_written);
BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(latfp);
  fclose_if_non_null(lonfp);
  fclose_if_non_null(midfp);
  return(status);
}
