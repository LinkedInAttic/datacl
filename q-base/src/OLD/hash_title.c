#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "fsize.h"

#include "title_types.h"
#include "hash_string.h"
#include "process_title.h"
#include "prime_geq.h"
#include "clean_title.h"
#include "hash_title.h"

int
hash_title(
	      char *in_title,
	      int *ptr_num_words,
	      wchar_t *first_char,
	      unsigned long *hash_word,
	      int max_num_words_in_title
	      )
{
  int status = 0;
  wchar_t word_in_title[MAX_TITLE_LEN];
  wchar_t wc_title[MAX_TITLE_LEN];
  char    mb_title[2*MAX_TITLE_LEN];
  unsigned long ultemp;
  wchar_t wc_nullc;
  int len, i, j, num_words, rval;
  int wit_ptr; /* word in title pointer */

  //----------------------------------------------
  wc_nullc = '\0';
  zero_string(mb_title, 2*MAX_TITLE_LEN);
  for( int i = 0; i < MAX_TITLE_LEN; i++ ) { 
    word_in_title[i] = wc_title[i] = '\0';
  }

  status = clean_title(in_title, wc_title, &len);
  cBYE(status);
  /* count num words and print first character of each word */

  first_char[0] = wc_title[0]; // 1st char of 1st word 
  wit_ptr = 0;
  for ( num_words = 0, i = 0, j = 0; i < len; i++ ) {
    if ( wc_title[i] == ' ' ) {
      first_char[num_words+1] = *(wc_title+i+1);
      rval = wcstombs(mb_title, word_in_title, 2*len);
      if ( rval < 0 ) { go_BYE(-1); }
      hash_string((char *)mb_title, &ultemp);
      hash_word[num_words] = ultemp;
      num_words++;
      word_in_title[wit_ptr++] = '\0';
      wit_ptr = 0;
      for( int i = 0; i < MAX_TITLE_LEN; i++ ) { 
	if ( word_in_title[i] == '\0' ) { break; }
	word_in_title[i] = '\0';
      }
      zero_string_to_nullc(mb_title);
    }
    else {
      if ( wit_ptr >= MAX_TITLE_LEN ) { go_BYE(-1); }
      word_in_title[wit_ptr++] = wc_title[i];
    }
  }
  //-- Deal with last word 
  word_in_title[wit_ptr++] = '\0';
  rval = wcstombs(mb_title, word_in_title, 2*len);
  if ( rval < 0 ) { go_BYE(-1); }
  hash_string(mb_title, &ultemp);
  hash_word[num_words] = ultemp;
  num_words++;

  *ptr_num_words = num_words;
 BYE:
  return(status);
}
