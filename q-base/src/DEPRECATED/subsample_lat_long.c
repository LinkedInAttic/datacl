#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "latlong_distance.h"

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *lat_file  = NULL;
  char *long_file = NULL;
  char *str_nout = NULL; int nout;
  char *opfile = NULL;
  char *X1 = NULL; size_t nX1 = 0;
  char *X2 = NULL; size_t nX2 = 0;
  float *lat = NULL, *lon = NULL;
  int nin, chk_nin, num_to_delete, num_deleted = 0;
  char *endptr;
  float dist, too_close_dist = 1;
  bool *is_deleted = NULL;
  FILE *ofp = NULL;

  if ( argc != 5 ) {
    fprintf(stderr, "Usage is %s <binary latfile> <binary long file> \
	<ascii lat/long opfile >\n", argv[0]); 
    go_BYE(-1); 
  }
  lat_file  = argv[1];
  long_file = argv[2];
  str_nout  = argv[3];
  opfile    = argv[4];
  //--------------------------------------------
  nout = strtoll(str_nout, &endptr, 10);
  if ( *endptr != '\0' ) { go_BYE(-1); }
  if ( nout <= 0 ) { go_BYE(-1); }
  //--------------------------------------------
  status = rs_mmap(lat_file, &X1, &nX1, 0); cBYE(status);
  nin = (int)(nX1 / sizeof(float));
  if ( ( nin * sizeof(float) ) != nX1 ) { go_BYE(-1); }
  lat = (float *)X1;
  //--------------------------------------------
  status = rs_mmap(lat_file, &X2, &nX2, 0); cBYE(status);
  chk_nin = (int)(nX2 / sizeof(float));
  if ( ( chk_nin * sizeof(float) ) != nX2 ) { go_BYE(-1); }
  lon = (float *)X2;
  //--------------------------------------------
  if ( nin != chk_nin ) { go_BYE(-1); }
  if ( nout >= nin ) { go_BYE(-1); }
  //--------------------------------------------
  ofp = fopen(opfile, "w");
  return_if_fopen_failed(ofp,  opfile, "w");
  //--------------------------------------------
  is_deleted = malloc(nin * sizeof(bool));
  return_if_malloc_failed(is_deleted);
  too_close_dist = 1;
  num_to_delete = nin - nout;
  for ( int i = 0; i < nin; i++ ) { 
    if ( is_deleted[i] ) { continue; }
    for ( int j = i+1; j < nin; j++ ) { 
      if ( is_deleted[j] ) { continue; }
      status = latlong_distance(lat[i], lon[i], lat[j], lon[j], &dist);
      if ( dist < too_close_dist ) { 
	is_deleted[j] = true;
	num_deleted++;
	if ( num_deleted >= num_to_delete ) { break; }
      }
    }
    if ( num_deleted >= num_to_delete ) { break; }
    too_close_dist++;
  }
  for ( int i = 0; i < nin; i++ ) { 
    if ( is_deleted[i] ) { continue; }
    fprintf(ofp, "%f,%f\n", lat[i], lon[i]);
  }
BYE:
  fclose_if_non_null(ofp);
  rs_munmap(X1, nX1);
  rs_munmap(X2, nX2);
}
