#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include "constants.h"
#include "macros.h"
#include "title_types.h"
#include "hash_title.h"

int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  wchar_t first_char[MAX_NUM_WORDS_IN_TITLE];
  unsigned long hash_word[MAX_NUM_WORDS_IN_TITLE];
  int num_words;

  if ( argc != 2 ) { go_BYE(-1); }

  hash_title(argv[1], &num_words, first_char, hash_word,
      MAX_NUM_WORDS_IN_TITLE);
  fprintf(stderr, "Num words = %d \n", num_words);
  for ( int i = 0; i < num_words; i++ ) { 
    fprintf(stderr, "%d\t%u\t\t%c\n", i, 
	(unsigned int)hash_word[i], 
	first_char[i]);
  }
BYE:
  return(status);
}
