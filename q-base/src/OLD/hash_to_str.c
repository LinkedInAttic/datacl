#include <stdio.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "title_types.h"
#include "hash_to_str.h"

int
hash_to_str(
	    HT3_TYPE *dict_ptr,
	    int num_dict_words,
	    unsigned int hash_val,
	    int *ptr_found,
	    wchar_t *wc_str_buffer /* length = MAX_CHARS_IN_WORD+1 */
	    )
{
  int status = 0;
  int found = FALSE;
  unsigned int dict_hash;

  if ( dict_ptr == NULL ) { go_BYE(-1); }
  if ( num_dict_words <= 0  ) { go_BYE(-1); }
  for ( int k = 0; k < num_dict_words; k++ ) { 
    dict_hash = dict_ptr[k].hash;
    if ( hash_val == dict_hash ) {
      for ( int l = 0; l < MAX_CHARS_IN_WORD; l++ ) {
	wc_str_buffer[l] = dict_ptr[k].word[l];
	if ( dict_ptr[k].word[l] == '\0' ) {
	  break;
	}
      }
      found = TRUE;
      break;
    }
  }
  if ( found == FALSE ) {  // Print hash instead
    swprintf(wc_str_buffer, 16, L"%u", (unsigned int)hash_val);
    /*
      fprintf(stderr, "ERROR: Could not find %u in dictionary\n", 
      (unsigned int)hash_val);
      go_BYE(-1);
    */
  }
  *ptr_found = found;
 BYE:
  return(status); 
}
