#include <stdio.h>
#include <math.h>
#include "constants.h"
#include "macros.h"
#include "latlong_distance.h"
#include "fsize.h"
#include "sort_asc_float.h"

#define MIN_POINTS 11 // Min number of points needed for averaging.
#define N 90 // We compute mu/sigma using N% of closest points
#define N2 4 // Number of sigmas away from mean considered too far

typedef struct rec_type { 
  float latitude;
  float longitude;
  int id;
} REC_TYPE;
 
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *infile = NULL, *outfile = NULL, *farguys = NULL;
  char *X = NULL; size_t nX = 0;
  int j, n;
  FILE *ofp = NULL, *fgfp = NULL; // fgfp = far guys file pointer 
  float *dist_from_center = NULL, *srt_dist_from_center = NULL;
  REC_TYPE *latlongid = NULL;
  float lat_sum, long_sum, lat_avg, long_avg, sum, mu, sigma, max_dist;
  int min_ok_dist; char *endptr = NULL;
  int cnt = 0;

  if ( argc != 5 ) { 
    fprintf(stderr, 
	    "Usage is %s <infile> <min ok dist> <outfile> <farguys> \n", argv[0]);
    go_BYE(-1);
  }
  infile  = argv[1];
  min_ok_dist  = strtol(argv[2], &endptr, 10);
  outfile = argv[3];
  farguys = argv[4];
  if ( strcmp(infile, outfile) == 0 ) { go_BYE(-1); }
  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  if ( nX == 0 ) { goto BYE; }
  n = nX / sizeof(REC_TYPE);
  if ( ( n * sizeof(REC_TYPE) ) != nX ) { go_BYE(-1); }
  latlongid = (REC_TYPE *)X;


  ofp = fopen(outfile, "wb");
  return_if_fopen_failed(ofp, outfile, "wb");
  if ( n < MIN_POINTS ) { 
    fwrite(X, sizeof(char), nX, ofp);
    fclose_if_non_null(ofp); 
    // Create zero sized file for outliers
    fgfp = fopen(farguys, "w");
    fclose_if_non_null(fgfp);
    goto BYE;
  }

  dist_from_center = (float *)malloc(n * sizeof(float));
  return_if_malloc_failed(dist_from_center);
  srt_dist_from_center = (float *)malloc(n * sizeof(float));
  return_if_malloc_failed(srt_dist_from_center);
  lat_sum = long_sum = 0;
  for ( int i = 0; i < n; i++ ) { 
    lat_sum += latlongid[i].latitude;
    long_sum += latlongid[i].longitude;
  }
  lat_avg = lat_sum / n;
  long_avg = long_sum / n;
  for ( int i = 0; i < n; i++ ) { 
    status = latlong_distance(lat_avg, long_avg, latlongid[i].latitude,
			      latlongid[i].longitude, dist_from_center+i);
    cBYE(status);
  }
  // Sort dist_from_center since we want to calculate mu/sigma using
  // only top N percent of the points
  for ( int i = 0; i < n; i++ ) { 
    srt_dist_from_center[i] = dist_from_center[i];
  }
  qsort(srt_dist_from_center, n, sizeof(float), sort_asc_float);
  j = n * (float)N / 100.0;
  if ( j == n ) { j--; }
  max_dist = srt_dist_from_center[j];

  // Calculate mean 
  sum = 0;
  for ( int i = 0; i < n; i++ ) { 
    if ( dist_from_center[i] > max_dist ) { continue; }
    sum += dist_from_center[i];
  }
  mu = sum / n;
  // Calculate variance 
  sum = 0;
  for ( int i = 0; i < n; i++ ) { 
    if ( dist_from_center[i] > max_dist ) { continue; }
    sum += sqr( dist_from_center[i] - mu ) ;
  }
  sigma = sqrt(sum / n);
  // fprintf(stderr, "n = %5d, mu = %f, sigma = %f \n", n, mu, sigma);
  // Corner case: It can happen that too many people are "too far". If
  // more than half the people are "too far", we say that nobody is too far
  cnt = 0;
  for ( int i = 0; i < n; i++ ) { 
    if ( ( dist_from_center[i] > (N2 * sigma) ) &&
         ( dist_from_center[i] > min_ok_dist ) ) {
      cnt++;
    }
  }
  fgfp = fopen(farguys, "w");
  return_if_fopen_failed(fgfp, farguys, "w");
  if ( cnt > (int)(n / 2.0) ) { // Nobody is far
    for ( int i = 0; i < n; i++ ) { 
      fwrite(&(latlongid[i].latitude),  sizeof(float), 1, ofp);
      fwrite(&(latlongid[i].longitude), sizeof(float), 1, ofp);
      fwrite(&(latlongid[i].id),        sizeof(int),   1, ofp);
    }
  }
  else {
    // Write out far guys and good guys
    for ( int i = 0; i < n; i++ ) { 
      int far_guy_idx;
      if ( ( dist_from_center[i] > (N2 * sigma) ) &&
	   ( dist_from_center[i] > min_ok_dist ) ) {
	cnt++;
	far_guy_idx = latlongid[i].id;
	fprintf(fgfp, "%d\n", far_guy_idx);
      }
      else {
	fwrite(&(latlongid[i].latitude),  sizeof(float), 1, ofp);
	fwrite(&(latlongid[i].longitude), sizeof(float), 1, ofp);
	fwrite(&(latlongid[i].id),        sizeof(int),   1, ofp);
      }
    }
  }
  fclose_if_non_null(fgfp);
  fclose_if_non_null(ofp);
  // fprintf(stderr, "Number too far away = %d \n", cnt);

 BYE:
  fclose_if_non_null(fgfp);
  fclose_if_non_null(ofp);
  free_if_non_null(dist_from_center);
  free_if_non_null(srt_dist_from_center);
  rs_munmap(X, nX);
  return(status);
}
