#include "qtypes.h"
#include "mmap.h"
#include "auxil.h"
#include "aux_meta.h"
#include "hash_string.h"

/* Given a multi-column tab separated file, it writes one binary file
 * for each column 
 */

/* TODO: Need to deal with null values */

#define MAXLINE 1048576

#define I4    1
#define I8    2
#define SUI4  3
#define STR   4
#define QSTR  5
// START FUNC DECL
int
tab_split(
	  char *infile,
	  char *in_flds,
	  char *in_fldtypes,
	  char *in_is_load,
	  char *str_splitchar
	  )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL;
  FILE **ofps = NULL;
  char *cptr;
  char *line = NULL;
  char *buffer = NULL;
  int itemp; long long lltemp; unsigned int uitemp;
  char **flds = NULL; int n_flds = 0, chk_n_flds = 0;
  char **str_fldtypes = NULL, **str_is_load = NULL;
  int *fldtypes  = NULL; bool *is_load = NULL;
  char splitchar; char ceoln = '\n';
  char dquote = '"'; char bslash = '\\';
  char *filename = NULL;

  if ( strcmp(str_splitchar, "tab") == 0 ) {
    splitchar = '\t';
  }
  else if ( strcmp(str_splitchar, "ctrl_z") == 0 ) {
    splitchar = 26;
  }
  else if ( strcmp(str_splitchar, "soh") == 0 ) {
    splitchar = 1;
  }
  else {
    go_BYE(-1);
  }
  if ( strlen(infile) == 0 ) { 
    ifp = stdin;
  }
  else { 
    ifp = fopen(infile, "r");
    return_if_fopen_failed(ifp, infile, "r");
  }

  status = explode(in_flds, ':', &flds, &n_flds);
  cBYE(status);
  for ( int i = 0; i < n_flds; i++ ) { 
    for ( int j = i+1; j < n_flds; j++ ) { 
      if ( strcmp(flds[i], flds[j]) == 0 ) { go_BYE(-1); }
    }
    // NOT DOING THIS BECUASE IT MEANS LINKING THIS CODE TO TIGHTLY WITH Q
    // status = chk_fld_name(flds[i]); cBYE(status);
  }
  status = explode(in_fldtypes, ':', &str_fldtypes, &chk_n_flds);
  cBYE(status);
  fldtypes = (int *)malloc(n_flds * sizeof(int));
  return_if_malloc_failed(fldtypes);
  if ( chk_n_flds != n_flds ) { 
    fprintf(stderr, "Fields named = %d \n", n_flds);
    fprintf(stderr, "Fields types = %d \n", chk_n_flds);
    go_BYE(-1); 
  }
  for ( int i = 0; i < n_flds ; i++ ) { 
    if ( strcmp(str_fldtypes[i], "I4") == 0 ) { 
      fldtypes[i] = I4;
    }
    else if ( strcmp(str_fldtypes[i], "I8") == 0 ) { 
      fldtypes[i] = I8;
    }
    else if ( strcmp(str_fldtypes[i], "SUI4") == 0 ) { 
      fldtypes[i] = SUI4;
    }
    else if ( strcmp(str_fldtypes[i], "STR") == 0 ) { 
      fldtypes[i] = STR;
    }
    else if ( strcmp(str_fldtypes[i], "QSTR") == 0 ) { 
      fldtypes[i] = QSTR;
    }
    else { 
      fprintf(stderr, "str_fldtypes[%d] = %s \n", i, str_fldtypes[i]); 
      go_BYE(-1); 
    }
  }

  status = explode(in_is_load, ':', &str_is_load, &chk_n_flds);
  cBYE(status);
  if ( chk_n_flds != n_flds ) { go_BYE(-1); }

  is_load = (bool *)malloc(n_flds * sizeof(bool));
  return_if_malloc_failed(is_load);
  for ( int i = 0; i < n_flds ; i++ ) { 
    if ( strcasecmp(str_is_load[i], "Y") == 0 ) { 
      is_load[i] = true;
    }
    else if ( strcasecmp(str_is_load[i], "N") == 0 ) { 
      is_load[i] = false;
    }
    else { go_BYE(-1); }
  }


  ofps = (FILE **)malloc(n_flds * sizeof(FILE *));
  return_if_malloc_failed(ofps);
  for ( int f = 0; f < n_flds; f++ ) { 
    ofps[f] = NULL;
    if ( is_load[f] == false ) { continue; }
    int len = strlen("_tempf_" ) + strlen(flds[f]);
    filename = malloc((len+4) * sizeof(char));
    return_if_malloc_failed(filename);
    strcpy(filename, "_tempf_");
    strcat(filename, flds[f]);
    ofps[f] = fopen(filename, "wb");
    return_if_fopen_failed(ofps[f], filename, "wb");
    free_if_non_null(filename);
  }

  line = malloc((MAXLINE) * sizeof(char));
  return_if_malloc_failed(line);
  buffer = malloc((MAX_LEN_STR+3) * sizeof(char));
  return_if_malloc_failed(buffer);
  zero_string(line, MAXLINE);
  zero_string(buffer, (MAX_LEN_STR+3));
  long long lno;
  for ( lno = 0; ; lno++ ) { 
    zero_string_to_nullc(line);
    // zero_string(line, MAXLINE);
    cptr = fgets(line, MAXLINE, ifp);
    if ( cptr == NULL ) { break; }
    if ( strlen(line) == 0 ) {
      fprintf(stderr, "Null line number %lld\n", lno); go_BYE(-1);
    }
    if ( line[strlen(line)-1] != '\n' ) {
      int ic; char c;
      // TODO: P2. Fix this.
      fprintf(stderr, "Line too long. Ignoring\n");
      // Skip over characters until you come to an eoln
      for ( ; !feof(ifp); ) { 
	ic = getc(ifp);
	c = (char)ic;
	if ( c == '\n' ) { 
	  break;
	}
      }
      continue;
    }
    char *cptr = line;
    bool is_eoln_seen = false;
    for ( int f = 0; f < n_flds; f++ ) { 
      if ( is_eoln_seen == true ) { 
	fprintf(stderr, "Reached end of line prematurely\n");
	go_BYE(-1);
      }
      int bufidx = 0;
      zero_string_to_nullc(buffer);
      // zero_string(buffer, (MAX_LEN_STR+3));
      bool nullify = false;
      for ( ;  ( ( *cptr != splitchar ) && ( *cptr != '\n') ); cptr++ ) { 
	if ( *cptr == '\0' ) { 
	  fprintf(stderr, "Unexpected string termination\n");
	  fprintf(stderr, "Line = %lld, Field = %d \n", lno, f);
	  go_BYE(-1);
	}
	// no need to load a buffer that will not be used 
	if ( is_load[f] == true ) { 
	  if ( bufidx >= MAX_LEN_STR ) { 
	    // We cannot deal with strings >= MAX_LEN_STR. Treat as null
	    nullify = true;
	  }
	  else {
	    buffer[bufidx++] = *cptr;
	  }
	}
      }
      if ( nullify == true ) { 
	zero_string_to_nullc(buffer);
        // zero_string(buffer, (MAX_LEN_STR+3));
      }
      if ( *cptr == '\n' ) { is_eoln_seen = true; }
      cptr++; // skip over delim
      // fprintf(stderr, "lno=%lld, f=%d, buffer= %s\n", lno, f, buffer);
      if ( is_load[f] == false ) {
	continue;
      }
      if ( splitchar == 1 ) { 
	if ( ( buffer[0] == '\\' ) && 
	     ( buffer[1] == 'N' ) && 
	     ( buffer[2] == '\0' ) ) { 
	  buffer[0] = buffer[1] = '\0';
	}
      }

      switch ( fldtypes[f] ) { 
      case STR  : 
	if ( buffer[0] != '\0' ) { 
	  // DBG if ( bufidx != strlen(buffer) ) { go_BYE(-1); }  
	  fwrite(buffer, bufidx, sizeof(char), ofps[f]);
	}
	fwrite(&ceoln, sizeof(char), 1, ofps[f]);
	break;
      case QSTR  : 
	fwrite(&dquote, 1, sizeof(char), ofps[f]);
	if ( buffer[0] != '\0' ) { 
	  // DBG if ( bufidx != strlen(buffer) ) { go_BYE(-1); }  
	  for ( int i = 0; i < bufidx; i++ ) { 
	    if ( ( buffer[i] == '\\' ) || ( buffer[i] == '"' ) ) {
  	      fwrite(&bslash, 1, sizeof(char), ofps[f]);
	    }
  	    fwrite(buffer+i, 1, sizeof(char), ofps[f]);
	  }
	}
  	fwrite(&dquote, 1, sizeof(char), ofps[f]);
	fwrite(&ceoln, sizeof(char), 1, ofps[f]);
	break;
      case I4 : 
	if ( buffer[0] == '\0' ) {
	  // TODO: P0 Need to fix this 
	  itemp = INT_MIN; 
	}
	else {
	  status = stoI4(buffer, &itemp); 
	  if ( status < 0 ) { 
	    fprintf(stderr, "Not I4: buffer = %s, lno = %lld \n",
		    buffer, lno);
	    go_BYE(-1);
	  }
	  cBYE(status);
	}
	fwrite(&itemp, 1, sizeof(int), ofps[f]);
	break;
      case SUI4 : 
	if ( buffer[0] == '\0' ) {
	  fprintf(stderr, "SUI4: Null value: Error on line %lld = [%s] \n", lno, line);
	  uitemp = 0; go_BYE(-1); 
	}
	else {
	  status = hash_string_UI4(buffer, &uitemp); cBYE(status);
	}
	fwrite(&uitemp, 1, sizeof(unsigned int), ofps[f]);
	break;
      case I8 : 
	if ( buffer[0] == '\0' ) {
	  // TODO: P0 Need to fix this 
	  lltemp = LLONG_MIN; 
	}
	else {
	  status = stoI8(buffer, &lltemp); cBYE(status);
	}
	fwrite(&lltemp, 1, sizeof(long long), ofps[f]);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
  }
  fprintf(stderr, "Wrote %lld lines \n",  lno);
 BYE:
  if ( strlen(infile) != 0 ) { 
    fclose_if_non_null(ifp);
  }
  if ( ofps != NULL ) { 
    for ( int f = 0; f < n_flds; f++ ) { 
      fclose_if_non_null(ofps[f]);
    }
    free_if_non_null(ofps);
  }
  free_if_non_null(line);
  free_if_non_null(buffer);
  free_if_non_null(filename);
  return(status);
}
