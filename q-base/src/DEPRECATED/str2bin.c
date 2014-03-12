#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "open_temp_file.h"
#include "fsize.h"

bool g_write_to_temp_dir = false;
/* TODO: This routine needs more work to deal with escaping dquotes and
 * backslashes and dealing with eoln within the quotes */
int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  char *infile = NULL;
  char *outfile_str = NULL;
  char *outfile_sz = NULL;
  char *outfile_nn = NULL;
  FILE *ofp = NULL, *szfp = NULL, *nnfp = NULL;
  char *X = NULL; size_t nX = 0;
  long long nR = 0, start_loc = -1, stop_loc =-1, len,fld_len;
  char nullc = '\0';
  bool is_in_str, pr_str;
  char c_one = 1; char c_zero = 0;

  if ( argc != 2 ) { go_BYE(-1); }
  infile      = argv[1];

  status = open_temp_file(&ofp, &outfile_str, 0); cBYE(status);
  status = open_temp_file(&szfp, &outfile_sz, 0); cBYE(status);
  status = open_temp_file(&nnfp, &outfile_nn, 0); cBYE(status);

  status = rs_mmap(infile, &X, &nX, 0);
  cBYE(status);
  nR = 0;
  is_in_str = false;
  pr_str = false;
  for ( size_t i = 0; i < nX; i++ ) {
    if ( X[i] == '"' ) {
      if ( is_in_str == false ) { 
        is_in_str = true;
        start_loc = i+1;
      }
      else {
        is_in_str = false;
        stop_loc = i-1;
	pr_str = true;
      }
    }
    if ( pr_str == true ) { 
      if ( ( stop_loc < 0 ) || ( start_loc < 0 ) || 
	  ( ( start_loc - stop_loc ) > 1 ) ) { 
	  fprintf(stderr, "Error at line %lld, position %lld of %lld \n", 
	      nR, (long long)i, (long long)nX);
	  go_BYE(-1);
      }
      nR++;
      pr_str = false;
      len = stop_loc - start_loc + 1; // + 1 bcos both are inclusive
      if ( len > 0 ) { 
        fwrite(X+start_loc, sizeof(char), len, ofp);
      }
      if ( len > 0 ) { 
        fwrite(&c_one, sizeof(char), 1, nnfp);
      }
      else {
        fwrite(&c_zero, sizeof(char), 1, nnfp);
      }
      fwrite(&nullc, sizeof(char), 1, ofp);
      fld_len = len + 1;
      fwrite(&fld_len, sizeof(int), 1, szfp);
      // Consume the eoln if it exists
      if ( i+1 < nX ) {
	if ( X[i+1] != '\n' ) { 
	  fprintf(stderr, "Error at line %lld, position %lld of %lld \n", 
	      nR, (long long)i, (long long)nX);
	  go_BYE(-1); 
	} 
	i++;
      }
    }
  }
  fprintf(stdout, "%lld:%s:%s:%s", nR, outfile_str, outfile_sz, outfile_nn);
BYE:
  fclose_if_non_null(ofp);
  fclose_if_non_null(szfp);
  fclose_if_non_null(nnfp);
  rs_munmap(X, nX);
  return(status);
}
