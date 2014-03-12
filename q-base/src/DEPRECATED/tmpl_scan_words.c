#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "macros.h"
#include "qtypes.h"
#include "sqlite3.h"
#include "fsize.h"
#include "auxil.h"
#include "hash_string.h"
// START FUNC DECL
int
scan_words___TYPE__(
	   bool is_first_pass, 
	   bool is_break_on_wspace,
	   char *src_fld_X, 
	   long long src_nR,
	   int *szptr,
	   __TYPE2__ *hashptr,
	   long long hash_nR,
	   FILE **ptr_ofp1, 
	   FILE **ptr_ofp2, 
	   FILE **ptr_ofp3,
	   long long *ptr_dst_nR
	   )
// STOP FUNC DECL
{
  int status = 0;
  char *cptr = NULL; long long sz = 0;
  long long dst_nR;
  char buffer[65536]; // FIX THIS HARD CODING TODO
  FILE *ofp1 = NULL, *ofp2 = NULL, *ofp3 = NULL;
  long long counter = 0; int strsz; 
  unsigned __TYPE2__ hash_val, curr_hash = 0; 
  char nullc = '\0';

  ofp1 = *ptr_ofp1;
  ofp2 = *ptr_ofp2;
  if ( is_first_pass ) { 
    ofp3 = *ptr_ofp3;
  }
  else {
    curr_hash = hashptr[counter];
  }
  zero_string(buffer, 65536);
  cptr = (char *)src_fld_X;
  dst_nR = 0;
  for ( long long i = 0; i < src_nR; i++ ) {  // for each row of source table 
    int bufptr = 0; // Set the buffer to 0
    zero_string_to_nullc(buffer); 
    if ( szptr == NULL ) { go_BYE(-1); } /* Need size of field */
    sz = szptr[i];
    if ( *cptr == '\0' ) { 
      if ( sz != 1 ) { go_BYE(-1); } /* double check data format */
      // Nothing to do for this row 
    }
    else {
      for ( int j = 0; ; j++ ) {
	if ( cptr[j] == '\0' ) { /* we have come to the end of field */
	  if ( j != (sz - 1) ) { go_BYE(-1); } /* double check data format */
	  if ( bufptr == 0 ) { /* Nothing to do */ break; }
	  if ( bufptr > 0 ) { 
	  }
	  // Cleaning the buffer may end up leaving nothing
	  if ( bufptr == 0 ) { /* Nothing to do */ break; }
	  if ( buffer[0] == '\0' ) { /* Nothing to do */ break; }
	  /* Now we have something to output */
	  status = __TYPE3__hash_string(buffer, &hash_val);
	  cBYE(status);
	  if ( is_first_pass ) { 
	    // Output the hash of the word and the row of occurrence
  	    fwrite(&hash_val,  1, sizeof(__TYPE2__), ofp1);
	    fwrite(&i,      1, sizeof(__TYPE2__), ofp2);
	    fwrite(&dst_nR, 1, sizeof(__TYPE2__), ofp3);
	  }
	  else {
	    if ( hash_val == curr_hash ) { /* print string and sz */
	      /* Quick double check of data format */
	      if ( counter > hash_nR ) { go_BYE(-1); } 
	      if ( counter < hash_nR ) { 
		fwrite(buffer, bufptr, sizeof(char), ofp1);
		fwrite(&nullc, 1,      sizeof(char), ofp1);
		strsz = bufptr + 1;
		fwrite(&strsz,  1, sizeof(int), ofp2);
		counter++;
		curr_hash = hashptr[counter];
	      }
	    }
	  }
	  dst_nR++;
	  zero_string_to_nullc(buffer);
	  bufptr = 0;
	  hash_val = 0;
	  break; /* We are done processing this row */
	}
	if ( ( is_break_on_wspace ) && ( isspace(cptr[j]) ) ) {
	  if ( bufptr == 0 ) { continue; }
	  if ( bufptr == 0 ) { continue; }
	  if ( buffer[0] == '\0' ) { /* Nothing to do */ break; }
	    status = __TYPE3__hash_string(buffer, &hash_val);
	    cBYE(status);
	    if ( is_first_pass ) { 
	      // Output the hash of the word and the row of occurrence
	      fwrite(&hash_val,  1, sizeof(int), ofp1);
	      fwrite(&i,      1, sizeof(int), ofp2);
	      fwrite(&dst_nR, 1, sizeof(int), ofp3);
	    }
	    else {
	      if ( hash_val == curr_hash ) { /* print string and sz */
		if ( counter < hash_nR ) { /* TODO: check boundary condition */
		  fwrite(buffer, bufptr, sizeof(char), ofp1);
		  fwrite(&nullc, 1,      sizeof(char), ofp1);
		  strsz = bufptr + 1;
		  fwrite(&strsz,  1, sizeof(int), ofp2);
		  counter++;
		  curr_hash = hashptr[counter];
		}
	      }
	    }
	    dst_nR++;
	    zero_string_to_nullc(buffer);
	    bufptr = 0;
	    hash_val = 0;
	}
	else {
	  buffer[bufptr++] = cptr[j];
	}
      }
    }
    cptr += sz;
  }
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  fclose_if_non_null(ofp3);
  *ptr_dst_nR = dst_nR;
  *ptr_ofp1 = ofp1;
  *ptr_ofp2 = ofp2;
  if ( is_first_pass ) { 
    *ptr_ofp3 = ofp3;
  }
 BYE:
  return(status);
}
