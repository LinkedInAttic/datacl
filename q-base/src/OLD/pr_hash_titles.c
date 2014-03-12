#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <wchar.h>

#include "constants.h"
#include "macros.h"
#include "title_types.h"
#include "fsize.h"
#include "prime_geq.h"
#include "hash_string.h"

int
pr_hash_titles(
    char *op_file_prefix,
    char *hash_words_in_title,
    char *num_words_per_title
    )
{
  int status = 0;
  int nw_i, itemp, num_titles, num_dict_words, len;
  char *fname = NULL;
  char *nw_X = NULL; int n_nw_X = 0; unsigned int *num_words = NULL; 
  char *hw_X = NULL; int n_hw_X = 0; unsigned int *hash_words = NULL;
  char *twdict_X = NULL; int n_twdict_X = 0; 
  HT3_TYPE *title_words = NULL; int num_title_words = 0;
  unsigned int *offset = NULL;
  wchar_t *fc_ptr = NULL;
  wchar_t wc_buf[2];

  wc_buf[0] = wc_buf[0] = '\0';
  //----------------------------------------------
  len = strlen(op_file_prefix) + 16;
  fname = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(fname);
  //-----------------------------------------------
  status = rs_mmap(num_words_per_title, &nw_X, &n_nw_X, 0);
  cBYE(status);
  num_titles = n_nw_X / sizeof(int);
  if ( ( num_titles * sizeof(int) ) != n_nw_X ) { go_BYE(-1); }
  num_words = (unsigned int *)nw_X;
  //-----------------------------------------------
  status = rs_mmap(hash_words_in_title, &hw_X, &n_hw_X, 0);
  cBYE(status);
  itemp = n_hw_X / sizeof(int);
  if ( ( itemp * sizeof(int) ) != n_hw_X ) { go_BYE(-1); }
  hash_words = (unsigned int *)hw_X;
  //-----------------------------------------------
  zero_string(fname, len);
  strcpy(fname, op_file_prefix);
  strcat(fname, ".twdict.bin");
  status = rs_mmap(fname, &twdict_X, &n_twdict_X, 0);
  cBYE(status);
  num_dict_words = n_twdict_X / sizeof(HT3_TYPE);
  if ( ( num_dict_words * sizeof(HT3_TYPE) ) != n_twdict_X ) { go_BYE(-1); }
  //-----------------------------------------------
  for ( int i = 0, wc = 0; i < num_titles; i++ ) { 
    hw_i = hash_words[i];
    status = hash_to_str(twdict)X, num_dict_words,  hw_i,
	&found, wc_str_buffer);

    nw_i = num_words[i];
    fprintf(stdout, "Title %d: NumWords = %d \n", i+1, nw_i);
    fprintf(stdout, "(");
    for ( int j = 0; j < nw_i; j++ ) { 
      wc_buf[0] = fc_ptr[j];
      fprintf(stdout, "%S -> %u ", wc_buf, hash_words[j]);
    }
    fprintf(stdout, ")\n");
    hash_words += nw_i;
    fc_ptr += (nw_i + 1 );
  }
BYE:
  free_if_non_null(offset);
  free_if_non_null(fname);
  rs_munmap(nw_X, n_nw_X);
  rs_munmap(hw_X, n_hw_X);
  rs_munmap(twdict_X, n_twdict_X);
  return(status);
}

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;

  if ( argc != 4 ) { 
    fprintf(stderr, "Usage is %s <output file prefix> <hash_words_in_title> <num_words_in_title> \n", argv[0]); 
    go_BYE(-1);
  }
  status = pr_hash_titles(argv[1], argv[2], argv[3]);
  cBYE(status);
 BYE:
  return(status);
}
