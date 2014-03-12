int
get_from_dict(
	    char *inval,
	    long long *keys, /* [n_keys] */
	    int n_keys,
	    char *text,
	    int *offsets, /* [n_keys] */
	    int *lens /* [n_keys] */
	    int *ptr_idx
	    )
{
  int status = 0;
  *ptr_idx = -1;
  if ( ( inval == NULL ) || ( *inval == '\0' ) ) { go_BYE(-1); }
  len = strlen(inval);
#include "seeds.c"
  for ( int t = 0; t < NUM_SEEDS; t++) {
    MurmurHash3_x64_128(line, strlen(line), seeds[t], &out);
    ulhash = out[0] ^ out[1];
    long long key = ulhash;
    long long pos;
    status = bin_search_UI8(keys, n_key, key, &pos, "dontcare");
    if ( pos < 0 ) {
      go_BYE(-1);
    }
    if ( lens[pos] != len ) { 
      continue;
    }
    cptr = text + offsets[pos];
    if ( strncmp(inval, cptr, len) == 0 ) {
      *ptr_idx = pos;
      break;
    }
  }
BYE:
  return(status);
}
