#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "mix.h"
#include "mmap.h"
#include "rd_dict.h"
#include "MurmurHash3.h"
#include "bin_search_UI8.h"


// START FUNC DECL
int
rd_dict(
    const unsigned long long *hashvals, /* input dictionary. Part 1 */
    int n_hashvals, /* size of input dictionary */
    const unsigned long long *len_offsets, /* input dictionary. Part 2 */
    const char *rawdata, /* concatenated strings */
    unsigned long long ulhash, /* value for which string is to be found */
    char *buffer, /* where output is stored if found */
    int sz_buffer, 
    unsigned int *ptr_len /* length of output */
    )
// STOP FUNC DECL
{
  int status = 0;
  long long pos;
  /* binary search to see if we can find hashval */
  *ptr_len = 0;
  status = bin_search_UI8(hashvals, n_hashvals, ulhash, &pos, NULL);
  cBYE(status);
  if ( pos < 0 ) { return(-1); }
  unsigned long long ultemp = len_offsets[pos];
  unsigned int len    = (ultemp >> 32 );
  unsigned int offset = (ultemp & 0xFFFFFFFF);
  *ptr_len = len;
  if ( len < sz_buffer ) {
    strncpy(buffer, rawdata+offset, len);
  }
BYE:
  return(status);
}
