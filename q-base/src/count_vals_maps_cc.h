#include <stdio.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  int count_vals_maps_cc (
			  int * x,
			  char * cfld,
			  long long siz,
			  FILE * idfp,
			  FILE * freqfp,
			  long long * out_siz
			  );

#ifdef __cplusplus
}
#endif
