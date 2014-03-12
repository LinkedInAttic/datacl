/* Usage is as follows. Input args are

   (1) infile - contains strings as input. Can be one column or 2 column
   file. If 2 column, then separator is ctrl_z, tab or comma and 1st
   column is integer.

   (2) ldb_path - path in which LevelDB dictionary is stored

   (3) ldb_dict - name of LevelDB dictionary 

   (4) access mode. Can be read/write/append. If read, then we assume
   that dictionary exists. If we find a string in input, that is not in
   LevelDB, then write it out as a null string.

   If write, then we destroy the LevelDB dictionary if it exists and
   create a new one, starting off empty.

   If append, then it is like read but if we find a string that is input
   and NOT in LevelDB, we add it to LevelDB.

   (4) write mode. Can be fixed_len/variable_len/leveldb. 

   If fixed_len, then we write out a single output file consisting of
   (nR*(maxlen+1)) bytes, where nR is number of rows in input and maxlen
   is maximum length of input. The strings are written in order i.e.,
   i^{th} string corresponds to string mapped to i. Hence, (maxlen+1)
   bytes will always be 0, indicating that 0 is null string. Note that if
   string is of length less than maxlen, it is padded with null
   characters. Name of output file is <op_prefix>_cstr

   If variable_len, then we write out 3 files. One contains the strings
   themselves, with a null termination character. Name of this file is
   <op_prefix>_vstr One contains lengths of strings. This is I4 in
   binary. Name of this file is <op_prefix>_len. Third contains offsets
   of strings. This is I4 in binary. Name of this file is <op_prefix>_off

   If leveldb, then ... TODO TBC

   (5) op_prefix

   We always create an output file with integers for each string, written
   in binary. Note that 0 represents a null string. Name of this file is
   <op_prefix>_I4


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"
#include "auxil.h"
#include "mmap.h"
#include "QLevelDB.h"
 
#define BUF_LEN 32768

#define FIXED_LEN    100
#define VARIABLE_LEN 200
#define LDB_OUTPUT   300
#define UNDEFINED    400

// START FUNC DECL
int
main(
     int argc,
     char **argv
     )
// STOP FUNC DECL
{
  int status = 0;
  int num_cols = 2;  // default. First is integer, second is string 
  // Currently, we assume that fld_sep is ctrlz or tab or comma
  // eoln is end of record 
  FILE *ifp      = NULL;
  char linebuf[BUF_LEN];
  int zero = 0; char nullc = '\0';
  char opfile_I4[MAX_LEN_FILE_NAME+1];   FILE *ofpI4 = NULL;
  char opfile_cstr[MAX_LEN_FILE_NAME+1]; FILE *ofpcstr = NULL;
  char opfile_vstr[MAX_LEN_FILE_NAME+1]; FILE *ofpvstr = NULL;
  char opfile_len[MAX_LEN_FILE_NAME+1];  FILE *ofplen = NULL;
  char opfile_off[MAX_LEN_FILE_NAME+1];  FILE *ofpoff = NULL;
  char opfile_idx[MAX_LEN_FILE_NAME+1];  FILE *ofpidx = NULL;

  int wr_mode; int max_str_len; // includes null character 
  int off = 0; // offset for variable length strings
  int idx = 0; // index for variable length strings

  int min_unused_value = 1;  //because 0 is for null string 
  bool not_found_is_error = false;

  // Process input arguments 
  if ( argc != 8 ) { go_BYE(-1); }
  char *infile   = argv[1]; 
  char *ldb_path = argv[2]; 
  char *ldb_dict = argv[3]; 
  char *rd_mode  = argv[4]; 

  if ( ( strcmp(rd_mode, "read") != 0 ) && 
       ( strcmp(rd_mode, "write") != 0 ) && 
       ( strcmp(rd_mode, "append") != 0 ) ) { go_BYE(-1); }

  if ( strcmp(rd_mode, "read") == 0 ) { 
    max_str_len = -1; // some junk value
    wr_mode = UNDEFINED;
  }
  else if ( ( strcmp(rd_mode, "append") == 0 ) || 
      ( strcmp(rd_mode, "write") == 0 ) ) { 
    status = stoI4(argv[5], &max_str_len); cBYE(status);
    if ( max_str_len == 0 ) { 
      wr_mode = LDB_OUTPUT;
      fprintf(stderr, "Not supported as yet\n"); go_BYE(-1); 
    }
    else if ( max_str_len < 0 ) { 
      wr_mode = VARIABLE_LEN;
    }
    else {
      wr_mode = FIXED_LEN;
    }
  }

  status = stoI4(argv[6], &num_cols); cBYE(status);
  if ( ( num_cols != 1 ) && ( num_cols != 2 ) )  { go_BYE(-1); }

  char *op_prefix   = argv[7]; 

  if ( strcmp(rd_mode, "write") == 0 ) {
    int dict_exists;
    status =  is_dict (ldb_dict, ldb_path, &dict_exists); cBYE(status);
    if ( dict_exists == 1 ) {
      fprintf(stderr, "Deleting existing dictionary\n");
      status = del_dict(ldb_dict, ldb_path); cBYE(status);
    }
    fprintf(stderr, "Creating new dictionary\n");
    status = mk_dict (ldb_dict, ldb_path); cBYE(status);
  }
  else if ( strcmp(rd_mode, "append") == 0 ) {
    int dict_exists;
    status =  is_dict (ldb_dict, ldb_path, &dict_exists); cBYE(status);
    if ( dict_exists != 1 ) { go_BYE(-1); }
  }

  if ( wr_mode == LDB_OUTPUT ) { 
    // Make sure that ldb dictionary exists 
    // need to figure out min unused value 
    // KISHORE TO FILL THIS OUT 
    go_BYE(-1); 
  }
  else if ( wr_mode == FIXED_LEN ) {
    // Done later
  }
  else if ( wr_mode == VARIABLE_LEN ) { 
    // Done later
  }
  else {
    go_BYE(-1);
  }

  if ( *infile == '\0' ) { go_BYE(-1); }
  if ( *op_prefix == '\0' ) { go_BYE(-1); }
  if ( ( strlen(op_prefix) + 5 ) >= MAX_LEN_FILE_NAME ) { go_BYE(-1); }
  // Open auxiliary files for storing data 
  char file_open_mode[4];
  if ( strcmp(rd_mode, "append") == 0 ) {
    strcpy(file_open_mode, "ab");
  }
  else if ( strcmp(rd_mode, "write") == 0 ) {
    strcpy(file_open_mode, "wb");
  }
  else {
    strcpy(file_open_mode, "--");
  }
  switch ( wr_mode ) { 
  case FIXED_LEN : 
    strcpy( opfile_cstr, op_prefix); strcat(opfile_cstr, ".cstr");
    ofpcstr = fopen(opfile_cstr, file_open_mode); 
    return_if_fopen_failed(ofpcstr, opfile_cstr, file_open_mode);

    if ( strcmp(file_open_mode, "wb") == 0 ) {
      // write the null string as string with ID 0
      for ( int i = 0; i < max_str_len; i++ ) { 
        fwrite(&nullc, sizeof(char), 1, ofpcstr); 
      }
      min_unused_value = 1;  // because 0 is for null string 
    }
    else {
      char *X = NULL;  size_t nX = 0; 
      status = rs_mmap(opfile_cstr, &X, &nX, 0); cBYE(status);
      min_unused_value = nX / max_str_len;
      if ( ( min_unused_value * max_str_len ) != nX ) { go_BYE(-1); }
      rs_munmap(X, nX);
    }
    break;
  case VARIABLE_LEN:  
    strcpy( opfile_vstr, op_prefix); strcat(opfile_vstr, ".vstr");
    ofpvstr = fopen(opfile_vstr, file_open_mode); 
    return_if_fopen_failed(ofpvstr, opfile_vstr, file_open_mode);

    strcpy( opfile_off,  op_prefix); strcat(opfile_off,  ".off");
    ofpoff = fopen(opfile_off, file_open_mode); 
    return_if_fopen_failed(ofpoff, opfile_off, file_open_mode);

    strcpy( opfile_idx,  op_prefix); strcat(opfile_idx,  ".idx");
    ofpidx = fopen(opfile_idx, file_open_mode); 
    return_if_fopen_failed(ofpidx, opfile_idx, file_open_mode);

    strcpy( opfile_len,  op_prefix); strcat(opfile_len,  ".len");
    ofplen = fopen(opfile_len, file_open_mode); 
    return_if_fopen_failed(ofplen, opfile_len, file_open_mode);

    if ( strcmp(file_open_mode, "wb") == 0 ) {
      // write the null string as string with ID 0
      int len = 1; off = 0; idx = 0;
      fwrite(&nullc, sizeof(char), 1, ofpvstr); 
      fwrite(&len, sizeof(int), 1, ofplen); 
      fwrite(&off, sizeof(int), 1, ofpoff); 
      fwrite(&idx, sizeof(int), 1, ofpidx); 
      off += len;
      idx++;
      min_unused_value = 1;  // because 0 is for null string 
    }
    else {
      char *lX = NULL;  size_t l_nX = 0; int *lI4 = NULL; int nL = 0;
      char *oX = NULL;  size_t o_nX = 0; int *oI4 = NULL; int nO = 0;
      status = rs_mmap(opfile_off, &oX, &o_nX, 0); cBYE(status);
      status = rs_mmap(opfile_len, &lX, &l_nX, 0); cBYE(status);
      nL = l_nX / sizeof(int); 
      if ( ( nL * sizeof(int) ) != l_nX ) { go_BYE(-1); }
      nO = o_nX / sizeof(int); 
      if ( ( nO * sizeof(int) ) != o_nX ) { go_BYE(-1); }
      if ( o_nX != l_nX ) { go_BYE(-1); }
      lI4 = (int *)lX;
      oI4 = (int *)oX;
      int last_o = oI4[nO-1];
      int last_l = lI4[nL-1];
      off = last_o + last_l;

      rs_munmap(oX, o_nX);
      rs_munmap(lX, l_nX);
      min_unused_value = nL; 
    }
    break;
  }
  // Open input file
  for ( int i = 0; i < BUF_LEN; i++ ) { linebuf[i] = '\0'; } 
  ifp = fopen(infile, "r"); 
  return_if_fopen_failed(ifp, infile, "r");

  // Open output file
  strcpy( opfile_I4, op_prefix); strcat(opfile_I4, ".I4");
  ofpI4 = fopen(opfile_I4, "wb");
  return_if_fopen_failed(ofpI4, opfile_I4, "wb");

  // Process input line by line 
  for ( long long lno = 0; !feof(ifp); lno++ ) {
    if ( fgets(linebuf, BUF_LEN-1, ifp) == NULL ) { break; }
    int len = strlen(linebuf);
    if ( linebuf[len-1] != '\n' ) { go_BYE(-1); }
    linebuf[len-1] = '\0'; // null terminate the string, eliminate elon

    if ( ( wr_mode == FIXED_LEN ) && ( len > max_str_len ) ) {
      fprintf(stderr, "String too long [%s] \n", linebuf);
      go_BYE(-1);
    }

    char *cptr = linebuf;
    char ctrl_z = 26;
    if ( num_cols == 2 ) { 
      for ( ; *cptr != '\0'; cptr++ ) {
        if ( ( *cptr == ',' ) || ( *cptr == '\t' ) || ( *cptr == ctrl_z ) ) {
  	  cptr++; 
        }
	else {
	  break;
	}
      }
      len = strlen(cptr);
    }
    if ( *cptr == '\0' ) { // null string 
      fwrite(&zero, sizeof(int), 1, ofpI4); 
    }
    else { // get integer from ldb and print it out 
      int ival = -1; int key_exists; 
      status = get_from_dict_2 (ldb_dict, ldb_path, cptr, &ival, &key_exists); 
      cBYE(status);
      if ( key_exists == 0 ) {
        if ( strcmp(rd_mode, "read") == 0 )  {
	  if ( not_found_is_error == true ) { 
	    fprintf(stderr, "Line %lld: String not found [%s]\n", lno, linebuf);
	    go_BYE(-1);
	  }
	  else {
	    ival = 0;
	  }
	}
	else {
	  status = add_to_dict_2 (ldb_dict, ldb_path, linebuf, min_unused_value);
	  cBYE(status);
	  ival = min_unused_value++;
	  //-----------------------------------------------------
	  switch ( wr_mode ) { 
	  case FIXED_LEN : 
	    fwrite(cptr, sizeof(char), len-1, ofpcstr); 
	    for ( int i = len-1; i < max_str_len; i++) { 
	      fwrite(&nullc, sizeof(char), 1, ofpcstr); 
	    }
	    break;
	  case VARIABLE_LEN : 
	    fwrite(cptr, sizeof(char), len, ofpvstr); 
	    fwrite(&len, sizeof(int), 1, ofplen); 
	    fwrite(&off, sizeof(int), 1, ofpoff); 
	    fwrite(&idx, sizeof(int), 1, ofpidx); 
	    off += len;  // note that len includes the terminating nullc
	    idx++;
	    break;
	  case LDB_OUTPUT : 
	    fprintf(stderr, "Not implemented\n");
	    go_BYE(-1);
	    break;
	  default : 
	    go_BYE(-1);
	    break;
	  }
	  //-----------------------------------------------------
	}
      }
      fwrite(&ival, sizeof(int), 1, ofpI4); 
    }
  }
 BYE:
  fclose_if_non_null(ifp);
  fclose_if_non_null(ofpI4);
  fclose_if_non_null(ofpcstr);
  fclose_if_non_null(ofpvstr);
  fclose_if_non_null(ofplen);
  fclose_if_non_null(ofpoff);
  fclose_if_non_null(ofpidx);
  return(status);
}
