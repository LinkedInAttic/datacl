#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "vec_f1f2opf3.h"

int
main()
{
  int status = 0;
  char *opfile = NULL; char *nn_opfile = NULL;
  long long nR = 4 * 1048576;
  int f1type = FLDTYPE_INT;
  int f2type = FLDTYPE_INT;
  char *str_meta_data = NULL;
  int *f1 = NULL, *f2 = NULL;
  char *nn_f1 = NULL, *nn_f2 = NULL;
  struct timeval *Tps = NULL, *Tpf = NULL;
  void *Tzp = NULL;
  unsigned long long int t_before_sec = 0, t_before_usec = 0, t_before = 0;
  unsigned long long int t_after_sec, t_after_usec, t_after;
  unsigned long long int t_delta_usec;

  f1 = (int *)malloc(nR * sizeof(int));
  f2 = (int *)malloc(nR * sizeof(int));
  nn_f1 = (char *)malloc(nR * sizeof(char));
  nn_f2 = (char *)malloc(nR * sizeof(char));

  for ( long long i = 0; i < nR; i++ ) { 
    f1[i] = i;
    f2[i] = 2*i;
    if ( ( i % 4 ) == 0 ) { 
      nn_f1[i] = TRUE; nn_f2[i] = TRUE;
    }
    else if ( ( i % 4 ) == 1 ) { 
      nn_f1[i] = TRUE; nn_f2[i] = FALSE;
    }
    else if ( ( i % 4 ) == 2 ) { 
      nn_f1[i] = FALSE; nn_f2[i] = TRUE;
    }
    else if ( ( i % 4 ) == 3 ) { 
      nn_f1[i] = FALSE; nn_f2[i] = FALSE;
    }
  }
  fprintf(stderr, "Starting \n");
  Tps = (struct timeval*) malloc(sizeof(struct timeval));
  Tpf = (struct timeval*) malloc(sizeof(struct timeval));
  Tzp = 0;
  gettimeofday ((struct timeval *)Tps, (struct timezone *)Tzp);
  t_before_sec  = (long long int)Tps->tv_sec;
  t_before_usec = (long long int)Tps->tv_usec;
  t_before = t_before_sec * 1000000 + t_before_usec;

  status = vec_f1f2opf3(nR, f1type, f2type, str_meta_data, 
      (char *)f1, nn_f1, (char *)f2, nn_f2, "+", &opfile, &nn_opfile);
  cBYE(status);
  gettimeofday ((struct timeval *)Tpf, (struct timezone *)Tzp); 
  t_after_sec  = (long long int)Tpf->tv_sec;
  t_after_usec = (long long int)Tpf->tv_usec;
  t_after = t_after_sec * 1000000 + t_after_usec;
  t_delta_usec = t_after - t_before;
  fprintf(stderr, "status = %d, time = %lld\n", status, t_delta_usec);
  fprintf(stderr, "Created output file = %s \n", opfile);
  if ( nn_opfile != NULL ) { 
    fprintf(stderr, "Created nn output file = %s \n", nn_opfile);
  }
BYE:
  free_if_non_null(opfile);
  free_if_non_null(nn_opfile);
  free_if_non_null(f1);
  free_if_non_null(f2);
  free_if_non_null(nn_f1);
  free_if_non_null(nn_f2);
  free_if_non_null(Tps);
  free_if_non_null(Tpf);
  return(status);
}
