#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "mmap.h"
#include "latlong_distance.h"
#include "levenshtein.h"


typedef struct latlongpop_type {
  float latitude;
  float longitude;
  int population;
} YESPOP_TYPE;

typedef struct latlong_type {
  float latitude;
  float longitude;
} NOPOP_TYPE;

#define BUFLEN 4096
#define MAX_LDIST 1

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *yespop_file = NULL;
  char *nopop_file    = NULL;
  char *pop_file         = NULL;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  int *pop = NULL;
  char *nn_pop = NULL;
  int nR1 = INT_MIN, nR2 = INT_MIN;
  YESPOP_TYPE *yespop = NULL;
  NOPOP_TYPE *nopop = NULL;
  char *endptr;
  bool is_some_null; char c_is_def;
  int my_pop, alt_pop;
  float my_lat, my_long, alt_lat, alt_long, dist, min_dist, max_dist;
  char *nn_pop_file = NULL; int len;
  FILE *ofp = NULL;
  char *yespop_desc_file = NULL;
  char *nopop_desc_file = NULL;
  char *yespop_desc = NULL; size_t n_yespop_desc = 0;
  char *nopop_desc = NULL; size_t n_nopop_desc = 0;
  char **yes_descs = NULL; char **no_descs = NULL;
  int yesptr, noptr; char c;
  char buffer[BUFLEN]; int ldist;
  char *dbg_file = NULL; FILE *dfp = NULL;

  zero_string(buffer, BUFLEN);
  if ( ( argc != 7 ) && ( argc != 8 ) ) { 
    fprintf(stderr, "Usage is %s <yespopfile> <nopofile> <yespop_desc> \
	<nopop_desc> <max_dist> <pop_file>  [dbgfile] \n", argv[0]); 
    go_BYE(-1); 
  }
  yespop_file = argv[1];
  nopop_file    = argv[2];
  yespop_desc_file = argv[3];
  nopop_desc_file = argv[4];
  max_dist = strtof(argv[5], &endptr);
  if ( max_dist <= 0 ) { go_BYE(-1); }
  pop_file        = argv[6];
  if ( argc == 8 ) { 
    dbg_file = argv[7];
  }
  if ( ( dbg_file != NULL ) && ( *dbg_file != '\0' ) ) {
    dfp = fopen(dbg_file, "w");
    return_if_fopen_failed(dfp, dbg_file, "w");
  }
  else { 
    dfp = stderr;
  }

  len = strlen(pop_file) + 8;
  nn_pop_file = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(nn_pop_file);
  zero_string(nn_pop_file, len);
  strcpy(nn_pop_file, "_nn_");
  strcat(nn_pop_file, pop_file);

  // mmap first file 
  status = rs_mmap(yespop_file, &X1, &nX1, 0);
  cBYE(status);
  nR1 = nX1 / sizeof(YESPOP_TYPE);
  if ( nR1 * sizeof(YESPOP_TYPE) != nX1 ) { go_BYE(-1); }
  if ( nR1 < 1 ) { go_BYE(-1); }

  // mmap second file 
  status = rs_mmap(nopop_file, &X2, &nX2, 0);
  cBYE(status);
  nR2 = nX2 / sizeof(NOPOP_TYPE);
  if ( nR2 * sizeof(NOPOP_TYPE) != nX2 ) { go_BYE(-1); }
  if ( nR2 < 1 ) { go_BYE(-1); }

  // allocate space for output
  pop = (int *)malloc(nR2 * sizeof(int));
  return_if_malloc_failed(pop);
  nn_pop = (char *)malloc(nR2 * sizeof(char));
  return_if_malloc_failed(nn_pop);

  yespop = (YESPOP_TYPE *)X1;
  nopop  = (NOPOP_TYPE *) X2;

  //----------------------------------------------------------
  status = rs_mmap(yespop_desc_file, &yespop_desc, &n_yespop_desc, 0);
  cBYE(status);

  yes_descs = (char **)malloc(nR1 * sizeof(char *));
  return_if_malloc_failed(yes_descs);
  for ( int i = 0; i < nR1; i++ ) { yes_descs[i] = NULL; }

  yesptr = 0;
  for ( int i = 0; i < nR1; i++ ) { 
    for ( int bufptr = 0; ; ) {
      if ( bufptr >= BUFLEN ) { 
	go_BYE(-1); 
      }
      c = yespop_desc[yesptr++];
      c = tolower(c);
      if ( isalpha(c) ) { // Eliminate non-alpha for matching purposes
        buffer[bufptr++] = c;
      }
      if ( c == '\0' ) { // string terminated
	yes_descs[i] = (char *)malloc(bufptr * sizeof(char));
	return_if_malloc_failed(yes_descs[i]);
	strcpy(yes_descs[i], buffer);
	zero_string_to_nullc(buffer);
	break;
      }
    }
  }
  if ( yesptr != n_yespop_desc ) { go_BYE(-1); }
  //----------------------------------------------------------
  status = rs_mmap(nopop_desc_file, &nopop_desc, &n_nopop_desc, 0);
  cBYE(status);

  no_descs = (char **)malloc(nR2 * sizeof(char *));
  return_if_malloc_failed(no_descs);
  for ( int i = 0; i < nR2; i++ ) { no_descs[i] = NULL; }

  noptr = 0;
  for ( int i = 0; i < nR2; i++ ) { 
    for ( int bufptr = 0; ; bufptr++ ) {
      if ( bufptr >= BUFLEN ) { 
	go_BYE(-1); 
      }
      c = nopop_desc[noptr++];
      buffer[bufptr] = tolower(c);
      if ( c == '\0' ) { // string terminated
	no_descs[i] = (char *)malloc((bufptr+1) * sizeof(char));
	return_if_malloc_failed(no_descs[i]);
	zero_string(no_descs[i], (bufptr+1));
	strcpy(no_descs[i], buffer);
	zero_string_to_nullc(buffer);
	break;
      }
    }
  }
  if ( noptr != n_nopop_desc ) { go_BYE(-1); }

  /* START: Check data */
  for ( int i1 = 0; i1  < nR1; i1++ ) { 
    if ( ( yespop[i1].latitude < -90 ) || 
	( yespop[i1].latitude > 90 ) ) { go_BYE(-1); } 
    if ( ( yespop[i1].longitude < -180 ) || 
	( yespop[i1].longitude > 180 ) ) { go_BYE(-1); }
  }

  for ( int i2 = 0; i2  < nR2; i2++ ) { 
    if ( ( nopop[i2].latitude < -90 ) || ( nopop[i2].latitude > 90 ) ) { 
      go_BYE(-1); 
    }

    if ( ( nopop[i2].longitude < -180 ) || ( nopop[i2].longitude > 180 ) ) { go_BYE(-1); }
  }
  /* STOP: Check data */

  is_some_null = false;
  for ( int i2 = 0; i2 < nR2; i2++ ) { 
    my_lat  = nopop[i2].latitude;
    my_long = nopop[i2].longitude;
    min_dist = 1e10;
    my_pop  = 0;
    c_is_def = FALSE;
    for ( int i1 = 0; i1 < nR1; i1++ ) { 
      alt_lat  = yespop[i1].latitude;
      alt_long = yespop[i1].longitude;
      alt_pop  = yespop[i1].population;
      // START: Approximate matching
      // Check that first character is the same
      if ( yes_descs[i1][0] != no_descs[i2][0] ) {
	continue;
      }
      // Check that descriptions approx the same 
      ldist = levenshtein_distance(yes_descs[i1], no_descs[i2]);
      if ( ldist > MAX_LDIST ) {
	continue;
      }
      // Check that physical distance is small
      status = latlong_distance(my_lat, my_long, alt_lat, alt_long, &dist);
      cBYE(status);
      if ( dist > max_dist ) {
	continue;
      }
	fprintf(dfp, "%5d: Matching %s to %s \t", i2, yes_descs[i1], no_descs[i2]);
	fprintf(dfp, "(%.3f, %.3f) --> (%.3f, %.3f) \n",
	    alt_lat, alt_long, my_lat, my_long);
	my_pop = alt_pop;
        c_is_def = TRUE;
	min_dist = dist;
    }
    if ( c_is_def == FALSE ) {
      fprintf(dfp, "%5d: No match for %s \n", i2, no_descs[i2]);
      is_some_null = true;
    }
    pop[i2] = my_pop;
    nn_pop[i2] = c_is_def;
    // fprintf(stderr, "DBG: Completed %d \n", i2);
  }
  ofp = fopen(pop_file, "wb");
  return_if_fopen_failed(ofp, pop_file, "wb");
  fwrite(pop, sizeof(int), nR2, ofp);
  fclose(ofp);

  if ( is_some_null ) { 
    ofp = fopen(nn_pop_file, "wb");
    return_if_fopen_failed(ofp, nn_pop_file, "wb");
    fwrite(nn_pop, sizeof(char), nR2, ofp);
    fclose(ofp);
  }

 BYE:
  if ( ( dbg_file != NULL ) && ( *dbg_file != '\0' ) ) {
    fclose_if_non_null(dfp);
  }
  rs_munmap(yespop_desc, n_yespop_desc);
  rs_munmap(nopop_desc, n_nopop_desc);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
  free_if_non_null(pop);
  free_if_non_null(nn_pop);
  free_if_non_null(yes_descs)
  free_if_non_null(no_descs)
  return(status);
}
