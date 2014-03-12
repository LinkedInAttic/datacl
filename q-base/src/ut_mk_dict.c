#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "qtypes.h"
#include "auxil.h"
#include "mix.h"
#include "mmap.h"
#include "mk_dict.h"

/* DO NOT AUTO GENERATE HEADER FILE  */
#define MAXLINE 65535
int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char line[MAXLINE+1];
  HT_REC_TYPE *ht = NULL;
  int sz_ht; // size of hash table 
  int n_ht = 0; // number of occupied entries in hash table 
  FILE *ifp = NULL;
  unsigned long long ulhash;
  char chrfile[MAX_LEN_FILE_NAME+1];
  char keyfile[MAX_LEN_FILE_NAME+1];
  char lenfile[MAX_LEN_FILE_NAME+1];
  char offfile[MAX_LEN_FILE_NAME+1];


  if ( argc != 3 ) { go_BYE(-1); }
  char *infile = argv[1];
  char *prefix = argv[2];

  zero_string(line, MAXLINE+1);
  strcpy(chrfile, prefix); strcat(chrfile, ".chr");
  strcpy(keyfile, prefix); strcat(keyfile, ".key");
  strcpy(lenfile, prefix); strcat(lenfile, ".len");
  strcpy(offfile, prefix); strcat(offfile, ".off");

  status = setup_ht_dict(&ht, &sz_ht, &n_ht); cBYE(status);

  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");
  for ( ; !feof(ifp); ) { 
    zero_string_to_nullc(line);
    char *cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    line[strlen(line)-1] = '\0'; // delete eoln 
    status = add_to_dict(line, ht, sz_ht, &n_ht, &ulhash); cBYE(status);
    fprintf(stderr, "Hashed %s to %llu\n", line, ulhash);
  }
  n_ht = 0;
  //--- print for debugging 
  for ( int i = 0; i < sz_ht; i++ ) { 
    if ( ht[i].cnt > 0 ) {
      fprintf(stderr, "%d:%d:%llu:%llu:%s\n", n_ht, i, ht[i].cnt, ht[i].key, ht[i].strkey);
      n_ht++;
    }
  }
  //--- print to create output structure
  status = dump_ht(ht, sz_ht, n_ht, chrfile, keyfile, lenfile, offfile); 
  cBYE(status);
 BYE:
  if ( ht != NULL ) { 
    for ( int i = 0; i < n_ht; i++ ) {
      free_if_non_null(ht[i].strkey);
    }
    free_if_non_null(ht);
  }
  fclose_if_non_null(ifp);
  return(status);
}
