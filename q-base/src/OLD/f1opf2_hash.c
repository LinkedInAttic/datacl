#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "constants.h"
#include "macros.h"
#include "hash_string.h"
#include "f1opf2_hash.h"

// START FUNC DECL
int
f1opf2_hash(
    char *hash_algo,
	    long long nR,
	    char *X,
	    size_t nX,
	    int *szptr,
	    int n_sizeof,
	    char *nn_X,
	    int hash_len,
	    FILE *ofp
	    )
// STOP FUNC DECL
{
  int status = 0;
  char *cptr = NULL; long long cum = 0;
  int ihash; unsigned int uihash; unsigned long long ullhash;
  int isz = sizeof(int); int llsz = sizeof(unsigned long long);
  

  cptr = X;
  for ( long long i = 0; i < nR; i++ ) {
    if ( ( cptr - X ) >= nX ) {
      fprintf(stderr, "cum = %lld, i = %lld \n", cum, i);
      go_BYE(-1);
    }
    switch ( hash_len ) {
    case 4 : 
      if ( hash_algo == NULL ) { 
        status = hash_string(cptr, &ihash); 
        cBYE(status);
	uihash = (unsigned int)ihash;
      }
      else {
	if ( strcmp(hash_algo, "addchars") == 0 ) { 
	  char *strptr = cptr; char c;
	  uihash = 0;
	  for ( int len = 1; *strptr != '\0'; len++, strptr++ ) { 
	    if ( len > isz ) { go_BYE(-1); }
	    c = *strptr;
	    if ( len > 1 ) { uihash = uihash << 8; }
	    uihash = uihash | c;
	  }
        }
        else {
  	  go_BYE(-1);
	}
      }
      fwrite(&uihash, sizeof(unsigned int), 1, ofp);
      break;
    case 8 : 
      if ( hash_algo == NULL ) { 
        status = ll_hash_string(cptr, &ullhash); 
        cBYE(status);
      }
      else {
	if ( strcmp(hash_algo, "addchars") == 0 ) { 
	  ullhash = 0;
	  char *strptr = cptr;
	  char c;
	  for ( int len = 1; *strptr != '\0'; len++, strptr++ ) { 
	    if ( len > llsz ) { 
	      go_BYE(-1); 
	    }
	    c = *strptr; 
	    if ( len > 1 ) { ullhash = ullhash << 8; }
	    ullhash = ullhash | c;
	  }
        }
        else {
  	  go_BYE(-1);
	}
      }

      fwrite(&ullhash, sizeof(unsigned long long), 1, ofp);
      break;
    default : 
      go_BYE(-1);
      break;
    }
    if ( szptr == NULL ) {
      cptr += n_sizeof;
      cum  += n_sizeof;
    }
    else {
      cptr += szptr[i];
      cum  += szptr[i];
    }
  }
 BYE:
  return(status);
}
