#include <stdio.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"

int
date_to_sec(
    char *str,
    int *ptr_t
    )
{
  int status = 0;
  if ( str == NULL ) { go_BYE(-1); }
#define NUM_PARTS 6
#define BUFLEN 16
  char tpart[NUM_PARTS][BUFLEN];
  int itpart[NUM_PARTS];
  char *cptr, *endptr;
  int t = 0;

  cptr = str;
  for ( int j = 0; j < NUM_PARTS; j++ ) { 
    zero_string(tpart[j], BUFLEN);
    for ( int i = 0; ( 
	( *cptr != '-') && ( *cptr != 'T' ) &&
	( *cptr != ':') && ( *cptr != '.' ) &&
	( *cptr != 'Z') 
	) ; cptr++ ) { 
      tpart[j][i++] = *cptr;
    }
    itpart[j] = strtol(tpart[j], &endptr, 10);
    cptr++;
    switch ( j ) { 
      case 0 : 
      case 1 : 
	break;
      case 2 : 
	t += itpart[j] * (24 * 3600);
	break;
      case 3 : 
	t += itpart[j] * 3600;
	break;
      case 4 : 
	t += itpart[j] * 60;
	break;
      case 5 : 
	t += itpart[j] * 1;
	break;
      default : 
	go_BYE(-1);
	break;
    }
  }
  *ptr_t = t;
  // fprintf(stderr, "%d ", t);
BYE:
  return(status);
}


#define NUMBUFS 4
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
  int m1;
  char bufs[NUMBUFS][MAXLINE];
  FILE* ofp[NUMBUFS];
  FILE* nn_ofp[NUMBUFS];
  char buffer[32];
  int t, mid, zero = 0; char nullc = '\0';
  char nntrue = 1, nnfalse = 0;

  for ( int i = 0; i < NUMBUFS; i++ ) { 
    zero_string(bufs[i], MAXLINE);

    sprintf(buffer, "_tempf_%d", i);
    ofp[i] = fopen(buffer, "wb");
    return_if_fopen_failed(ofp[i], buffer, "wb");

    sprintf(buffer, "_nn_tempf_%d", i);
    nn_ofp[i] = fopen(buffer, "wb");
    return_if_fopen_failed(nn_ofp[i], buffer, "wb");
  }
  zero_string(line, MAXLINE);
  if ( argc != 2 ) { go_BYE(-1); }
  infile = argv[1];
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( ; !feof(ifp); ) { 
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
	  mid = strtol(bufs[j], &endptr, 10);
	  if ( *endptr != '\0' ) { go_BYE(-1); }
	  fwrite(&mid, sizeof(int), 1, ofp[j]);
	  break;
	case 1 : 
	  mid = strtol(bufs[j], &endptr, 10);
	  if ( *endptr != '\0' ) { go_BYE(-1); }
	  if ( strlen(bufs[j]) == 0 ) { 
	    fwrite(&zero, sizeof(int), 1, ofp[j]);
	    fwrite(&nnfalse, sizeof(char), 1, nn_ofp[j]);
	  }
	  else {
	    fwrite(&mid, sizeof(int), 1, ofp[j]);
	    fwrite(&nntrue, sizeof(char), 1, nn_ofp[j]);
	  }
	  break;
	case 2 : 
	case 3 : 
	  if ( strlen(bufs[j]) == 0 ) { go_BYE(-1); }
	  status = date_to_sec(bufs[j], &t);
	  fwrite(&t, sizeof(int), 1, ofp[j]);
	  break;
	default : 
	  go_BYE(-1);
	  break;
      }
      zero_string_to_nullc(bufs[j]);
    }
    zero_string_to_nullc(line);
  }
BYE:
  for ( int i = 0; i < NUMBUFS; i++ ) { 
    fclose_if_non_null(ofp[i]);
    fclose_if_non_null(nn_ofp[i]);
  }
  fclose_if_non_null(ifp);
  return(status);
}
