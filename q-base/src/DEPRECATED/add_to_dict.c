int
add_to_dict(
	    char *inval,
	    HT_REC_TYPE *ht
	    )
{
  int status = 0;
#include "seeds.c"
  for ( int t = 0; t < NUM_SEEDS; t++) {
    MurmurHash3_x64_128(line, strlen(line), seeds[t], &out);
    ulhash = out[0] ^ out[1];
    if ( strcmp(line, "abc") == 0 ) {
      ulhash = 1111;
    }
    else if ( strcmp(line, "def") == 0 ) {
      if ( t == 0 ) { 
	ulhash = 1111;
      }
      else  if ( t == 1 ) { 
	ulhash = 2222;
      }
    }
    else if ( strcmp(line, "ghi") == 0 ) {
      if ( t == 0 ) { 
	ulhash = 1111;
      }
      else  if ( t == 1 ) { 
	ulhash = 2222;
      }
      else  if ( t == 2 ) { 
	ulhash = 3333;
      }
    }
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
	if ( strcmp(ht[i].strkey, line) == 0 ) {
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

      if ( ht[i].cnt == 0 ) { // unoccupied 
	pos_ht = i;
	key_exists = false;
	space_found = true;
	break;
      }
    }
    if ( pick_new_hash == true ) { continue; }
    if ( space_found == false ) {
      for ( int i = 0; i < ht_loc; i++ ) { 
	num_probes++;
	if ( ht[i].key == ulhash ) {
	  if ( strcmp(ht[i].strkey, line) == 0 ) {
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
	if ( ht[i].cnt == 0 ) { // unoccupied 
	  pos_ht = i;
	  key_exists = true;
	  space_found = true;
	  break;
	}
	if ( ht[i].cnt == 0 ) { // unoccupied 
	  pos_ht = i;
	  key_exists = false;
	  space_found = true;
	  break;
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
      ht[pos_ht].strkey = strdup(line);
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
    ht[pos_ht].strkey = strdup(line);
    n_ht++;
  }
  if ( n_ht > ( sz_ht >> 1 ) ) {
    fprintf(stderr, "TO BE IMPLEMENTED\n");
    go_BYE(-1);
  }
 BYE:
  return(status);
}
