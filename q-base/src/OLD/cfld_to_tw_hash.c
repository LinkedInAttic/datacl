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
#include "bin_search.h"

int
cfld_to_tw_hash(
	   char *op_file_pre,
	   char *in_cfld,
	   char *out_fld,
	   char *num_words_in_title,
	   int *ptr_num_titles_matched,
	   int *ptr_num_words_matched
	   )
{ 
  int status = 0;
  char *fname = NULL;
  char *cfld_X = NULL; int n_cfld_X; 
  char *nw_X = NULL; int n_nw_X; unsigned int *num_words = NULL;
  char *hw_X = NULL; int n_hw_X; unsigned int *hash_words = NULL;
  int len, offset, itemp, num_titles;
  int num_titles_matched = 0, num_words_matched = 0;
  FILE *ofp = NULL, *nwfp = NULL;
  unsigned int nw_i;

  //-----------------------------------------------
  if ( ( out_fld == NULL ) || ( strlen(out_fld) == 0 ) ) { go_BYE(-1); }
  if ( ( in_cfld == NULL ) || ( strlen(in_cfld) == 0 ) ) { go_BYE(-1); }
  if ( ( op_file_pre == NULL ) || ( strlen(op_file_pre) == 0 ) ) { go_BYE(-1); }
  //-----------------------------------------------
  ofp = fopen(out_fld, "wb");
  return_if_fopen_failed(ofp, out_fld, "wb");
  nwfp = fopen(num_words_in_title, "wb");
  return_if_fopen_failed(nwfp, num_words_in_title, "wb");
  //-----------------------------------------------
  len = strlen(op_file_pre) + strlen(in_cfld) + 4;
  fname = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(fname);
  zero_string(fname, len);
  strcpy(fname, op_file_pre);
  strcat(fname, in_cfld);
  status = rs_mmap(fname, &cfld_X, &n_cfld_X, 0);
  cBYE(status);
  num_titles = n_cfld_X / sizeof(char);
  if ( ( num_titles * sizeof(char) ) != n_cfld_X ) { go_BYE(-1); }
  free_if_non_null(fname);
  //-----------------------------------------------
  len = strlen(op_file_pre) + strlen(".nw.bin") + 4;
  fname = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(fname);
  zero_string(fname, len);
  strcpy(fname, op_file_pre);
  strcat(fname, ".nw.bin");
  status = rs_mmap(fname, &nw_X, &n_nw_X, 0);
  cBYE(status);
  itemp = n_nw_X / sizeof(int);
  if ( ( itemp * sizeof(int) ) != n_nw_X ) { go_BYE(-1); }
  if ( itemp != num_titles ) { go_BYE(-1); }
  free_if_non_null(fname);
  num_words = (unsigned int *)nw_X;
  //-----------------------------------------------
  len = strlen(op_file_pre) + strlen(".hw.bin") + 4;
  fname = (char *)malloc(len * sizeof(char));
  return_if_malloc_failed(fname);
  zero_string(fname, len);
  strcpy(fname, op_file_pre);
  strcat(fname, ".hw.bin");
  status = rs_mmap(fname, &hw_X, &n_hw_X, 0);
  cBYE(status);
  itemp = n_hw_X / sizeof(int);
  if ( ( itemp * sizeof(int) ) != n_hw_X ) { go_BYE(-1); }
  free_if_non_null(fname);
  hash_words = (unsigned int *)hw_X;
  //-----------------------------------------------
  offset = 0;
  for ( int i = 0; i < num_titles; i++ ) {
    nw_i = num_words[i];
    if ( cfld_X[i] == TRUE ) { 
      fwrite(hash_words+offset, sizeof(unsigned int), nw_i, ofp);
      fwrite(&nw_i, sizeof(unsigned int), 1, nwfp);
      num_titles_matched++;
      num_words_matched++;
    }
    offset += nw_i;
  }
  *ptr_num_titles_matched = num_titles_matched;
  *ptr_num_words_matched = num_words_matched;
 BYE:
  rs_munmap(nw_X, n_nw_X);
  rs_munmap(hw_X, n_hw_X);
  fclose_if_non_null(ofp);
  fclose_if_non_null(nwfp);
  free_if_non_null(fname);
  return(status);
}

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  int num_titles_matched = 0;
  int num_words_matched = 0;

  if ( argc != 5 ) { 
    fprintf(stderr, "Usage is %s <output file prefix> <in_cfld> <out_hash_words> <out_num_words_per_title>\n", argv[0]);
    go_BYE(-1);
  }
  status = cfld_to_tw_hash(argv[1], argv[2], argv[3], argv[4],
      &num_titles_matched, &num_words_matched);
  cBYE(status);
  fprintf(stdout, "%d,%d\n", num_titles_matched, num_words_matched);
 BYE:
  return(status);
}
