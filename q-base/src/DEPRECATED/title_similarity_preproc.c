#include <stdio.h> 
#include <limits.h> 
#include "constants.h"
#include "macros.h"
#include "fsize.h"
#include "hash_string.h"

#define BMASK 0xFFFFFFFFFFFF0000
#define NOT_BMASK 0x000000000000FFFF

extern int str_to_id(
	      char *str, 
	      char **titles,
	      int num_titles,
	      unsigned long long *hash_table,
	      int *ptr_id
	      );
static int XXXcompare(
		      const void *ii, 
		      const void *jj
		      );
extern int
load_title_file(
		char *title_file,
		char ***ptr_titles,
		int *ptr_num_titles,
		unsigned long long **ptr_hash_table
		);

#define BUFLEN 1024
#define MAXLINE (2*BUFLEN)

int
main(
     int argc,
     char **argv
     )
{
  int status = 0;
  char *infile = NULL;
  char line[MAXLINE];
  FILE *ifp = NULL; 
  FILE *ofp1 = NULL, *ofp2 = NULL, *ofp3 = NULL, *ofp4 = NULL, *ofp5 = NULL;
  char *endptr = NULL;
  char from_title_buffer[BUFLEN]; char to_title_buffer[BUFLEN];
  int from_tid, to_tid;
  float fprob; int iprob;
  char **titles = NULL; int num_titles = 0;
  unsigned long long *hash_table = NULL;
  unsigned long long from_to_tid, from_to_iprob;
  int n_from_tid = 0;
  int n_lines_skipped = 0;

  zero_string(from_title_buffer, BUFLEN);
  zero_string(to_title_buffer, BUFLEN);

  if ( argc != 3 ) { go_BYE(-1); }

  infile = argv[1];
  ifp = fopen(infile, "r");
  return_if_fopen_failed(ifp, infile, "r");

  ofp1 = fopen("_tempf_from_tid", "wb");
  return_if_fopen_failed(ofp1, "xxx", "wb");
  ofp2 = fopen("_tempf_iprob", "wb");
  return_if_fopen_failed(ofp2, "xxx", "wb");
  ofp3 = fopen("_tempf_to_tid", "wb");
  return_if_fopen_failed(ofp3, "xxx", "wb");
  ofp4 = fopen("_tempf_fprob", "wb");
  return_if_fopen_failed(ofp4, "xxx", "wb");
  ofp5 = fopen("_tempf_from_to_iprob", "wb");
  return_if_fopen_failed(ofp5, "xxx", "wb");

  status = load_title_file(argv[2], &titles, &num_titles, &hash_table);
  cBYE(status);
  fprintf(stderr, "Loaded %d titles \n", num_titles);

  for ( int lno = 1; !feof(ifp); lno++ ) { 
    fgets(line, MAXLINE, ifp);
    if ( *line == '\0' ) { break; }
    if ( line[0] != '\t' ) { /* Starting a new from_title */
      zero_string_to_nullc(from_title_buffer);
      for ( int i = 0; line[i] != '\n'; i++ ) { 
	from_title_buffer[i] = line[i];
      }
      n_from_tid++;
      fprintf(stderr, "From %dth title = %s \n", n_from_tid, from_title_buffer);
      status = str_to_id(from_title_buffer, titles, num_titles,
	  hash_table, &from_tid);
      cBYE(status);
    }
    else {
      zero_string_to_nullc(to_title_buffer);
      int fld_marker = 0;
      int i, j;
      for ( i = 0, j = 0; line[i] != '0'; i++ ) { 
	if ( line[i] == '\t' ) { 
	  continue;
	}
	to_title_buffer[j++] = line[i];
	fld_marker = i;
      }
      fld_marker++;
      for ( int i = strlen(to_title_buffer) - 1; i >= 0; i-- ) { 
	if ( to_title_buffer[i] == ' ' ) {
	  to_title_buffer[i] = '\0';
	}
	else {
	  break;
	}
      }
      // fprintf(stderr, "To title = %s \n", to_title_buffer);
      status = str_to_id(to_title_buffer, titles, num_titles,
	  hash_table, &to_tid);
      if ( status < 0 ) { 
	n_lines_skipped++;
	continue;
      }

      fprob = 0;
      fprob = strtof(line + fld_marker, &endptr);
      if ( ( fprob <= 0 ) || ( fprob >= 1 ) ) {
	fprintf(stderr, "Error on Line %d \n", lno);
	go_BYE(-1); 
      }
      iprob = fprob * 1024; // Computed to 3 decimal places 
      from_to_tid = ( from_tid << 16 ) | to_tid;
      from_to_iprob = ( from_to_tid << 32 ) | iprob;
      fwrite(&from_tid, sizeof(int), 1, ofp1); // for sorting
      fwrite(&iprob, sizeof(int), 1, ofp2); // for sorting
      fwrite(&to_tid, sizeof(int), 1, ofp3);
      fwrite(&fprob, sizeof(float), 1, ofp4);
      fwrite(&from_to_iprob, sizeof(long long), 1, ofp5);
    }
    zero_string_to_nullc(line);
  }
 BYE:
  for ( int i = 0; i < num_titles; i++ ) { 
    free_if_non_null(titles[i]);
  }
  free_if_non_null(titles);
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofp1);
  fclose_if_non_null(ofp2);
  fclose_if_non_null(ofp3);
  fclose_if_non_null(ofp4);
  fclose_if_non_null(ofp5);
  return(status);
}

#define MAX_TITLE_LEN 256
int
load_title_file(
		char *title_file,
		char ***ptr_titles,
		int *ptr_num_titles,
		unsigned long long **ptr_hash_table
		)
{
  int status = 0;
  FILE *tfp = NULL;
  char title[MAX_TITLE_LEN];
  int num_titles = 0, max_title_len = 0;
  char **titles = NULL;
  unsigned long long ll_hash, l_i, l_j;
  unsigned long long *hash_table = NULL;
  char *cptr = NULL;

  //--- Count number of titles
  tfp = fopen(title_file, "r");
  return_if_fopen_failed(tfp, title_file, "r");
  zero_string(title, MAX_TITLE_LEN);
  for ( ; !feof(tfp); num_titles++ ) {
    int len;
    fgets(title, MAX_TITLE_LEN, tfp);
    if ( *title == '\0' ) { break; }
    len = strlen(title);
    if ( len > max_title_len ) { 
      max_title_len = len;
    }
    zero_string_to_nullc(title);
  }
  fclose_if_non_null(tfp);
  max_title_len++;
  //----------------------------
  // Allocate space for titles
  titles = (char **)malloc(num_titles * sizeof(char *));
  return_if_malloc_failed(titles);
  for ( int i = 0; i < num_titles; i++ ) { 
    titles[i] = (char *)malloc(max_title_len * sizeof(char));
    return_if_malloc_failed(titles[i]);
  }
  hash_table = (unsigned long long *)malloc(num_titles * sizeof(unsigned long long));
  return_if_malloc_failed(hash_table);

  //----------------------------
  tfp = fopen(title_file, "r");
  return_if_fopen_failed(tfp, title_file, "r");
  zero_string(title, MAX_TITLE_LEN);
  for ( int title_index = 0; title_index < num_titles; title_index++ ) { 
    if ( feof(tfp) ) { go_BYE(-1); }
    fgets(title, MAX_TITLE_LEN, tfp);
    cptr = title;
    for ( int j = 0; *cptr != ','; j++, cptr++ ) { 
      titles[title_index][j] = *cptr;
    }
    ll_hash = 0;
    status = ll_hash_string(titles[title_index], &ll_hash);
    cBYE(status);
    ll_hash = ll_hash & BMASK;
    ll_hash = ll_hash | title_index;
    hash_table[title_index] = ll_hash;
    zero_string_to_nullc(title);
  }
  fclose_if_non_null(tfp);
  for ( int i = 0; i < num_titles; i++ ) { 
    l_i = hash_table[i] & BMASK;
    for ( int j = i+1; j < num_titles; j++ ) { 
      l_j = hash_table[j] & BMASK;
      if ( l_i == l_j ) { 
	fprintf(stderr, "Same hash for (%d, %d) titles %s and %s \n",
		i, j, titles[i], titles[j]);
	hash_table[i] = hash_table[j] = 0; // Force sequential search
      }
    }
  }
  qsort(hash_table, num_titles, sizeof(long long), XXXcompare);

  *ptr_num_titles = num_titles;
  *ptr_titles = titles;
  *ptr_hash_table = hash_table;
 BYE:
  fclose_if_non_null(tfp);
  return(status);
}

static int XXXcompare(
		      const void *ii, 
		      const void *jj
		      )
{ 
  unsigned long long val1, val2;
  unsigned long long *ptr1, *ptr2;
  ptr1 = (unsigned long long *)ii;
  ptr2 = (unsigned long long *)jj;
  val1 = *ptr1 & 0xFFFFFFFFFFFF0000;
  val2 = *ptr2 & 0xFFFFFFFFFFFF0000;

  /* Output in asc order */
  if ( val1 > val2 )  {
    return (1);
  }
  else if ( val1 < val2 ) {
    return (-1);
  }
  else {
    return(0);
  }
}

int str_to_id(
	      char *str, 
	      char **titles,
	      int num_titles,
	      unsigned long long *hash_table,
	      int *ptr_id
	      )
{
  int status = 0;
  unsigned long long hashval;
  int id = -1;
  int len;

  // Delete trailing spaces
  len = strlen(str);
  for ( int i = len - 1; i >= 0; i-- ) { 
    if ( str[i] ==' ' ) { 
      str[i] = '\0'; 
    }
    else {
      break;
    }
  }

  status = ll_hash_string(str, &hashval);
  cBYE(status);
  hashval = hashval & BMASK; 
  // TODO: Use binary sort or hash table direct access
  for ( int h = 0; h < num_titles; h++ ) { 
    if ( hashval == ( hash_table[h] & BMASK ) ) {
      id = hash_table[h] & NOT_BMASK;
      // fprintf(stderr, "Matched Title %d = %s \n", id, titles[id]);
      break;
    }
  }
  if ( id < 0 ) { /* Fall back to sequential search */
    // fprintf(stderr, "Using sequential search for %s \n", str);
    for ( int h = 0; h < num_titles; h++ ) { 
      if ( strcmp(titles[h], str) == 0 ) { 
	id = h;
	break;
      }
    }
  }
  if ( id < 0 ) { 
    // fprintf(stderr, "Unable to find title %s \n", str);
    go_BYE(-1);
  }
  *ptr_id = id;
 BYE:
  return(status);
}
