#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "macros.h"
#include "auxil.h"
/* parse following for file names and lengths */
// START FUNC DECL
int
get_file_names(
	       const char *in_str,
	       int *ptr_num_files,
	       char ***ptr_filenames,
	       long long **ptr_file_lengths
	       )
// STOP FUNC DECL
{
  int status = 0;
  int num_files = 0;
  char **filenames = NULL;
  long long *file_lengths = NULL;
  char *lstr = NULL;
  char *str, *cptr;
  int n1, n2;
#define BUFLEN 127
  char buffer[BUFLEN+1];

  zero_string(buffer, BUFLEN+1);
  if ( ( in_str == NULL ) || ( *in_str == '\0' ) )  { go_BYE(-1); }
  lstr = strdup(in_str);
  str = strstr(lstr, "[");
  if ( str == NULL ) { fprintf(stderr, "in_str=%s\n", in_str);go_BYE(-1); }
  str++; // jump over open square bracket
  if ( isspace(*str) ) { str++; } // jump over space if exists
  cptr = strstr(str+1, "]");
  if ( cptr == NULL ) { go_BYE(-1); }
  *cptr = '\0';
  // Now str is a null terminated string that contains what we want
  status = count_char(str, '{', &n1); cBYE(status);
  status = count_char(str, '}', &n2); cBYE(status);
  if ( n1 != n2 ) { go_BYE(-1); }
  num_files = n1; 
  filenames = malloc(num_files * sizeof(char *));
  return_if_malloc_failed(filenames);
  file_lengths = malloc(num_files * sizeof(long long));
  return_if_malloc_failed(file_lengths);
  char *save_str = str;
  for ( int mode = 1; mode <= 2; mode++ ) { 
    str = save_str;
    char start_str[32]; zero_string(start_str, 32);
    switch ( mode ) { 
    case 1 : strcpy(start_str, "\"length\":"); break;
    case 2 : strcpy(start_str, "\"pathSuffix\":"); break;
    default : go_BYE(-1); break;
    }
    for ( int i = 0; i < n1; i++ ) {
      cptr = strstr(str, start_str);
      if ( cptr == NULL ) { go_BYE(-1); }
      cptr += strlen(start_str);
      if ( *cptr == '"' ) { cptr++; }
      int j;
      zero_string(buffer, BUFLEN+1);
      for ( j = 0; ( ( *cptr != ',' ) && ( *cptr != '"' ) ); j++ ) { 
	if ( j >= BUFLEN ) { go_BYE(-1); }
	if ( *cptr == '\0' ) { go_BYE(-1); }
	buffer[j] = *cptr++;
      }
      buffer[j] = '\0';
      // fprintf(stderr, "buffer = %s \n", buffer);
      switch ( mode ) { 
      case 1 : 
	status = stoI8(buffer, &(file_lengths[i])); 
	if ( file_lengths[i] <= 0 ) { go_BYE(-1); }
	break;
      case 2 : 
	filenames[i] = strdup(buffer);
	if ( filenames[i] == NULL ) { go_BYE(-1); }
	break;
      default : 
	go_BYE(-1);
	break;
      }
      // Advance to next guy
      str = cptr;
    }
  }

 BYE:
  free_if_non_null(lstr);
  *ptr_num_files    = num_files;
  *ptr_filenames    = filenames;
  *ptr_file_lengths = file_lengths;
  return status ;
}

#undef UNIT_TEST
#ifdef UNIT_TEST
/*
  gcc -g -Wall -std=gnu99 mmap.c auxil.c ./AUTOGEN/GENFILES/assign_I1.c aux_hdfs.c -I./AUTOGEN/GENFILES -lm
*/
int
main(
    )
{
  int status = 0;
  int num_files = 0;
  char **filenames = NULL;
  long long *file_lengths = NULL;
  char *str =  "{\"FileStatuses\":{\"FileStatus\":[ {\"accessTime\":1382717082417,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391578039,\"modificationTime\":1382635532885,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00000\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635546318,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391540247,\"modificationTime\":1382635546318,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00001\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635541391,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391657383,\"modificationTime\":1382635541391,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00002\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635531152,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391446957,\"modificationTime\":1382635531152,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00003\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635535311,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391603859,\"modificationTime\":1382635535311,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00004\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635551781,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391353893,\"modificationTime\":1382635551781,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00005\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635541631,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391538073,\"modificationTime\":1382635541631,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00006\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635556884,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391674634,\"modificationTime\":1382635556884,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00007\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635530770,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391592293,\"modificationTime\":1382635530770,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00008\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"}, {\"accessTime\":1382635535240,\"blockSize\":536870912,\"group\":\"rsubramo\",\"length\":391675087,\"modificationTime\":1382635535240,\"owner\":\"rsubramo\",\"pathSuffix\":\"part-r-00009\",\"permission\":\"640\",\"replication\":3,\"type\":\"FILE\"} ]}}";

  status = get_file_names(str, &num_files, &filenames, &file_lengths);
  cBYE(status);
  for ( int i = 0; i < num_files; i++ ) { 
    fprintf(stderr, "%d: %lld : %s \n", i, file_lengths[i], filenames[i]);
  }
  free_if_non_null(file_lengths);
  if ( filenames != NULL ) { 
    for ( int i = 0; i < num_files; i++ ) { 
      free_if_non_null(filenames[i]);
    }
    free_if_non_null(filenames);
  }
 BYE:
  return status ;
}

 
#endif
