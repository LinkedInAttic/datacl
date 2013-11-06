/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "qtypes.h"
#include "auxil.h"
#include "mix.h"
#include "mmap.h"
#include "mk_dict.h"
#include "MurmurHash3.h"
#include "bin_search_I8.h"

extern char g_cwd[MAX_LEN_DIR_NAME+1];

// START FUNC DECL
int hop_sort_compare(
		     const void *in1, 
		     const void *in2
		     )
// STOP FUNC DECL
{ 
  HOP_REC_TYPE *hop1, *hop2;

  hop1 = (HOP_REC_TYPE *)in1;
  hop2 = (HOP_REC_TYPE *)in2;

  // IMPORTANT: We sort assuming that hash value is signed.
  long long hash1 = (long long)(hop1->key);
  long long hash2 = (long long)(hop2->key);


  /* Output in ascending order */
  if ( hash1 > hash2 ) {
    return (1);
  }
  else if ( hash1 < hash2 ) { 
    return (-1);
  }
  else {
    return(0);
  }
}

// START FUNC DECL
int
setup_ht_dict(
	      HT_REC_TYPE **ptr_ht, 
	      int *ptr_sz_ht, 
	      int *ptr_n_ht
	      )
// STOP FUNC DECL
{
  int status = 0;
  int sz_ht;
  HT_REC_TYPE *ht = NULL;

  sz_ht = prime_geq(4*1048576);
  ht = (HT_REC_TYPE *)malloc(sz_ht * sizeof(HT_REC_TYPE));
  return_if_malloc_failed(ht);
  for ( int i = 0; i < sz_ht; i++ ) { 
    ht[i].strkey = NULL;
    ht[i].key = 0;
    ht[i].cnt = 0;
  } 
  *ptr_ht = ht;
  *ptr_sz_ht = sz_ht;
  *ptr_n_ht = 0;
 BYE:
  return(status);
}
  
// START FUNC DECL
int
add_to_dict(
	    char *inval,
	    HT_REC_TYPE *ht,
	    int sz_ht,
	    int *ptr_n_ht,
	    unsigned long long *ptr_outval

	    )
// STOP FUNC DECL
{
  int status = 0;
  unsigned long long out[2], ulhash;
  int ht_loc, pos_ht; bool space_found, key_exists;
  long long num_probes = 0; long long max_count = 0;
  bool pick_new_hash; int n_ht = INT_MAX;

  n_ht = *ptr_n_ht;
#include "seeds.c"
  for ( int t = 0; t < NUM_SEEDS; t++) {
    MurmurHash3_x64_128(inval, strlen(inval), seeds[t], &out);
    ulhash = out[0] ^ out[1];
    // fprintf(stderr, ":%llu:%llu:%llu\n", ulhash, out[0], out[1]);
    ht_loc = mix_I8(ulhash) % sz_ht;
    pos_ht = -1;
    // Determine whether you have seen this guy before or not
    // If not, then you also need to find the position to put him
    pick_new_hash = false;
    space_found = false;
    for ( int i = ht_loc; i < sz_ht; i++ ) {
      num_probes++;
      if ( ht[i].key == ulhash ) {
	if ( strcmp(ht[i].strkey, inval) == 0 ) {
	  pos_ht = i;
	  key_exists = true;
	  space_found = true;
	  break;
	}
	else {
	  pick_new_hash = true; 
	  break;
	}
      }
      else {
	if ( ht[i].cnt == 0 ) { // unoccupied 
	  pos_ht = i;
	  key_exists = false;
	  space_found = true;
	  break;
	}
      }
    }
    if ( space_found   == true ) { break;    }
    if ( pick_new_hash == true ) { continue; }
    if ( space_found == false ) {
      for ( int i = 0; i < ht_loc; i++ ) { 
	num_probes++;
	if ( ht[i].key == ulhash ) {
	  if ( strcmp(ht[i].strkey, inval) == 0 ) {
	    pos_ht = i;
	    key_exists = true;
	    space_found = true;
	    break;
	  }
	  else {
	    pick_new_hash = true; 
	    break;
	  }
	}
	else {
	  if ( ht[i].cnt == 0 ) { // unoccupied 
	    pos_ht = i;
	    key_exists = false;
	    space_found = true;
	    break;
	  }
	}
      }
    }
    if ( space_found == true ) { break; }
  }
  //------------------------------------------------------------
  if ( space_found == false ) { go_BYE(-1); }
  if ( pos_ht < 0 ) { go_BYE(-1); }
  if ( key_exists ) {
    if ( ht[pos_ht].key != ulhash ) { 
      go_BYE(-1);
    }
    if ( pos_ht == 1 ) {
      go_BYE(-1);
    }
    if ( ht[pos_ht].cnt == 0 ) {
      ht[pos_ht].cnt = 1;
      ht[pos_ht].key = ulhash;
      ht[pos_ht].strkey = strdup(inval);
      n_ht++;
    }
    ht[pos_ht].cnt = ht[pos_ht].cnt + 1;
    if ( ht[pos_ht].cnt > max_count ) { 
      max_count = ht[pos_ht].cnt;
    }
  }
  else {
    ht[pos_ht].cnt = 1;
    ht[pos_ht].key = ulhash;
    ht[pos_ht].strkey = strdup(inval);
    n_ht++;
  }
  if ( n_ht > ( sz_ht >> 1 ) ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n");
    go_BYE(-1);
  }
  *ptr_n_ht = n_ht;
  *ptr_outval = ulhash;
 BYE:
  return(status);
}
//--- print to create output structure
// START FUNC DECL
int dump_ht(
	    HT_REC_TYPE *ht,
	    int sz_ht,
	    int n_ht,
	    char *data_dir,
	    char *chrfile,
	    char *keyfile,
	    char *lenfile,
	    char *offfile
	    )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ofp1 = NULL,  *ofp2 = NULL,  *ofp3 = NULL, *ofp4 = NULL;
  char *X = NULL; size_t nX;
  unsigned int len = 0;
  unsigned int offset = 0;
  HOP_REC_TYPE *hop = NULL;

  if ( ( data_dir != NULL ) && ( *data_dir != '\0' ) ) {  
    chdir(data_dir); 
  } 


  ofp1 = fopen(chrfile, "wb"); return_if_fopen_failed(ofp1, chrfile, "wb");
  ofp2 = fopen(keyfile, "wb"); return_if_fopen_failed(ofp2, keyfile, "wb");
  ofp3 = fopen(lenfile, "wb"); return_if_fopen_failed(ofp3, lenfile, "wb");
  ofp4 = fopen(offfile, "wb"); return_if_fopen_failed(ofp4, offfile, "wb");

  len = 0;
  hop = malloc(n_ht * sizeof(HOP_REC_TYPE));
  return_if_malloc_failed(hop);
  int hopctr = 0;
  for ( int i = 0; i < sz_ht; i++ ) { 
    if ( ht[i].cnt > 0 ) {
      len = strlen(ht[i].strkey);
      hop[hopctr].key = ht[i].key;
      hop[hopctr].len = len;
      hop[hopctr].orig_idx = i;
      hopctr++;
    }
  }
  status = rs_mmap(chrfile, &X, &nX, 0); cBYE(status);
  qsort(hop, n_ht, sizeof(HOP_REC_TYPE), hop_sort_compare);

  for ( int i = 0; i < n_ht; i++ ) { 
    unsigned int len    = hop[i].len;
    unsigned long long key = hop[i].key;
    unsigned int orig_idx    = hop[i].orig_idx;
    fwrite(ht[orig_idx].strkey, sizeof(char), (len+1), ofp1);
    fwrite(&key, sizeof(unsigned long long), 1, ofp2);
    fwrite(&len,          sizeof(int),                1, ofp3);
    fwrite(&offset,       sizeof(int),                1, ofp4);
    offset += (len+1);
  }
  chdir(g_cwd);
 BYE:
  free_if_non_null(hop);
  rs_munmap(X, nX);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  fclose_if_non_null(ofp3);
  fclose_if_non_null(ofp4);
  return(status);
}

int
get_from_dict(
	      char *inval,
	      long long *keys, /* [n_keys] */
	      int n_keys,
	      char *text,
	      int *offsets, /* [n_keys] */
	      int *lens, /* [n_keys] */
	      int *ptr_idx
	      )
{
  int status = 0;
  unsigned long long out[2], ulhash;

  if ( ( inval == NULL ) || ( *inval == '\0' ) ) { go_BYE(-1); }
  *ptr_idx = -1;
  int len = strlen(inval);
#include "seeds.c"
  for ( int t = 0; t < NUM_SEEDS; t++) {
    MurmurHash3_x64_128(inval, len, seeds[t], &out);
    ulhash = out[0] ^ out[1];
    long long key = ulhash;
    long long pos;
    status = bin_search_I8(keys, n_keys, key, &pos, "dontcare");
    if ( pos < 0 ) {
      // fprintf(stderr, "Unable to find value %s in dictionary \n", inval);
      break; 
    }
    if ( lens[pos] != len ) { 
      continue;
    }
    char *cptr = text + offsets[pos];
    if ( strncmp(inval, cptr, len) == 0 ) {
      *ptr_idx = pos;
      break;
    }
  }
 BYE:
  return(status);
}
