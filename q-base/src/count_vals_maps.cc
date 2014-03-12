#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <map>
#include "count_vals_maps_cc.h"
#include "macros.h"

using namespace std;

int count_vals_maps_cc (
			int *x,
			char * cfld,
			long long siz,
			FILE * idfp,
			FILE * freqfp,
			long long * out_siz 
			)
{
  
  int status = 0;

  std::map <int, int> key_val_map;
  
  if ( cfld == NULL ) {

    for ( long long ii = 0; ii < siz; ii++ ) {
      key_val_map[x[ii]]++;
    }

  }

  else {

    for ( long long ii = 0; ii < siz; ii++ ) {
      if ( cfld[ii] == 0 ) { continue; }
      key_val_map[x[ii]]++;
    }

  }

  map<int,int>::iterator iter = key_val_map.begin();
  
  int temp_val, temp_key;
  long long count_iter = 0;

  while ( iter != key_val_map.end() ) {
    
    temp_key = iter->first;
    temp_val = iter->second;
    
    fwrite(&temp_key, sizeof(int), 1, idfp);
    fwrite(&temp_val, sizeof(int), 1, freqfp);
    iter++;
    count_iter++;

  }

  *out_siz = count_iter;

  return (status);

}
