#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include "QLevelDB.h"
#include "qtypes.h"
#include "auxil.h"
#include "aux_meta.h"
#include "dbauxil.h"
#include "extract_I4.h"

#define READ   100
#define WRITE  200
#define APPEND 300

#define BUFLEN 16

static  int 
setup_dict(
	   char *dict,
	   char *data_dir,
	   int mode
	   )
{
  int status = 0; 
  int dict_exists;
  //-----------------------------------------------------------------
  status = is_dict(dict, data_dir, &dict_exists);
  cBYE(status);
  switch ( mode ) { 
  case READ : case APPEND : 
    if ( dict_exists != 1 ) { go_BYE(-1); } 
    break; 
  case WRITE : 
    if ( dict_exists == 1 ) {
      fprintf(stderr, "Deleting ldb dict %s from %s \n", dict, data_dir);
      status = del_dict(dict, data_dir); cBYE(status);
    }
    status = mk_dict(dict, data_dir); cBYE(status);
    status = is_dict(dict, data_dir, &dict_exists); cBYE(status);
    if ( dict_exists != 1 ) { go_BYE(-1); }
    break;
  default : 
    go_BYE(-1); break;
  }
 BYE:
  return(status);
}

// START FUNC DECL
int 
mk_ldb_dict(
	    char *infile,
	    char *str_fld_sep,
	    char *ldb_dict,
	    char *str_mode, /* read, write, append */
	    char *joinfile, /* to store join key if provided */
	    char *outfile, /* to store outputs as I4 */
	    char *options, 
	    char *in_q_data_dir
	    )
// STOP FUNC DECL
{
  int status = 0;
  FILE *ifp = NULL, *ofp = NULL, *jfp = NULL;
  char fld_sep; char rec_sep = '\n';
  char fwd_ldb_dict[MAX_LEN_TBL_NAME+1];
  char rev_ldb_dict[MAX_LEN_TBL_NAME+1];
  char q_data_dir[MAX_LEN_DIR_NAME+1];
  int mode;

  //-----------------------------------------------------------------
  zero_string(q_data_dir, MAX_LEN_DIR_NAME+1);
  zero_string(fwd_ldb_dict, MAX_LEN_TBL_NAME+1);
  zero_string(rev_ldb_dict, MAX_LEN_TBL_NAME+1);
  if ( ( outfile != NULL ) && ( *outfile != '\0' ) ) { 
    ofp = fopen(outfile, "wb"); 
    return_if_fopen_failed(ofp,  outfile, "wb"); 
  }
  if ( ( joinfile != NULL ) && ( *joinfile != '\0' ) ) { 
    jfp = fopen(joinfile, "wb"); 
    return_if_fopen_failed(jfp,  joinfile, "wb"); 
  }
  if ( ( infile == NULL ) || ( *infile == '\0' ) ) { 
    ifp = stdout;
  }
  else {
    ifp = fopen(infile, "r"); 
    return_if_fopen_failed(ifp,  infile, "r"); 
  }
  if ( ( str_fld_sep == NULL ) || ( *str_fld_sep == '\0' ) ) { 
    fld_sep = '\t';
  }
  else {
    if ( strcmp(str_fld_sep, "tab") == 0 ) {
      fld_sep = '\t';
    }
    else if ( strcmp(str_fld_sep, "comma") == 0 ) {
      fld_sep = ',';
    }
    else if ( strcmp(str_fld_sep, "ctrl_z") == 0 ) {
      fld_sep = 26;
    }
    else {
      go_BYE(-1);
    }
  }
  //--------------------------------------------------
  if ( ( str_mode == NULL ) || ( *str_mode == '\0' ) ) { go_BYE(-1); }
  if ( strcasecmp(str_mode, "read") == 0 ) { 
    mode =  READ;
  }
  else if ( strcasecmp(str_mode, "write") == 0 ) { 
    mode =  WRITE;
  }
  else if ( strcasecmp(str_mode, "append") == 0 ) { 
    mode =  APPEND;
  }
  else {
    go_BYE(-1);
  }

  if ( ( in_q_data_dir == NULL ) || ( *in_q_data_dir == '\0' ) ) { 
    go_BYE(-1);
  }
  if ( is_directory(in_q_data_dir) == false ) { cBYE(status); }
  int len = strlen(in_q_data_dir);
  if ( len > (MAX_LEN_DIR_NAME-1) ) { go_BYE(-1); }
  strcpy(q_data_dir, in_q_data_dir);
  if ( in_q_data_dir[len-1] != '/' ) { 
    strcat(q_data_dir, "/");
  }
  //--------------------------------------------------
  if ( ( ldb_dict == NULL ) || ( *ldb_dict == '\0' ) ) { 
    go_BYE(-1);
  }
  {
    int len = 0;
    for ( char *cptr = ldb_dict; *cptr != '\0'; cptr++ ) { 
      if ( !isalnum(*cptr) ) { 
	go_BYE(-1);  
      }
      len++;
      if ( ( len > MAX_LEN_TBL_NAME-4 ) ) { go_BYE(-1); }
    }
  }
  //--------------------------------------------------
  int num_cols = 2;
  if ( ( options != NULL ) && ( *options != '\0' ) ) { 
    bool is_null; 
    status = chk_aux_info(options); cBYE(status);
    status = extract_I4(options, "num_cols=[", "]", &num_cols, &is_null); 
    cBYE(status);
    if ( is_null == false ) { 
      if ( ( num_cols != 1 ) && ( num_cols != 2 ) ) { go_BYE(-1); }
    }
  }

  //--------------------------------------------------
  strcpy(fwd_ldb_dict, "fwd."); strcat(fwd_ldb_dict, ldb_dict); 
  strcpy(rev_ldb_dict, "rev."); strcat(rev_ldb_dict, ldb_dict); 
  status = setup_dict(fwd_ldb_dict, q_data_dir, mode); cBYE(status);
  status = setup_dict(rev_ldb_dict, q_data_dir, mode); cBYE(status);

  int min_unused_val = 1; 
  if ( mode == APPEND ) { 
    // TODO P1: If we store maxval, then we do not have to iterate for it
    int first_exists, next_exists; int itemp, maxval = 0;
    char key[MAX_LEN_STR]; char nextkey[MAX_LEN_STR]; char val[MAX_LEN_STR];
    status = get_first_key_val_from_dict_1(fwd_ldb_dict, q_data_dir, 
	key, MAX_LEN_STR, val, BUFLEN, &first_exists);
    cBYE(status);
    status = stoI4(val, &itemp); cBYE(status);
    if ( itemp > maxval ) { 
      maxval =  itemp;
    }
    if ( first_exists == 0 ) { go_BYE(-1); }
    for ( ; ; ) { 
      status = get_next_key_val_from_dict_1(fwd_ldb_dict, q_data_dir, 
	key, nextkey, MAX_LEN_STR, val, BUFLEN, &next_exists);
      cBYE(status);
      if ( next_exists == 0 ) { break; }
      strcpy(key, nextkey);
      status = stoI4(val, &itemp); cBYE(status);
      if ( itemp > maxval ) { 
        maxval =  itemp;
      }
    }
    min_unused_val = maxval + 1;
  }
  //
  //-----------------------------------------------------------------
  int state;
  char keybuf[MAX_LEN_STR];
  char valbuf[BUFLEN], joinbuf[BUFLEN]; 
  zero_string(keybuf, MAX_LEN_STR); 
  int keyidx = 0; int joinidx = 0;
  zero_string(valbuf, BUFLEN); 
  zero_string(joinbuf, BUFLEN); 
#define READING_1st_COL 10
#define READING_2nd_COL 20

	switch ( num_cols ) { 
	  case 1 : state = READING_2nd_COL; break; 
	  case 2 : state = READING_1st_COL; break;
	  default : go_BYE(-1); break;
	}

  long long lno;
  long long last_lno = -1;
  for ( lno = 1; !feof(ifp); ) { 
    int ic = getc(ifp);
    if ( ic < 0 ) { 
      
      break; 
    }
    switch ( state ) { 
    case READING_1st_COL : 
      if ( ic == fld_sep ) { 
	state = READING_2nd_COL;
	if ( joinidx > 0 ) { 
	  if ( jfp != NULL ) { 
	    long long joinI8;
	    status = stoI8(joinbuf, &joinI8); cBYE(status);
	    fwrite(&joinI8, sizeof(long long), 1, jfp);
	  }
	  zero_string_to_nullc(joinbuf); 
	}
	zero_string(keybuf,  MAX_LEN_STR); keyidx = 0;
	zero_string(joinbuf, MAX_LEN_STR); joinidx = 0;
      }
      else {
	if ( joinidx > BUFLEN ) { go_BYE(-1); }
	joinbuf[joinidx++] = (char)ic;
      }
      break;
    case READING_2nd_COL : 
      if ( ic == rec_sep ) { 
	int key_exists;
	state = READING_1st_COL;
	zero_string(valbuf, BUFLEN); 
	status = get_from_dict_1(fwd_ldb_dict, q_data_dir, keybuf, 
				 valbuf, BUFLEN, &key_exists);
	cBYE(status);
	int val_to_write;
	if ( key_exists == 0  ) {
	  if ( mode == READ ) {
	    fprintf(stderr, "Key %s not found \n", keybuf); 
	    go_BYE(-1); 
	  }
	  /* the key does not exist in the dictionary, will be added to 
	   * it with a new integer not used by anyone else */
	  sprintf(valbuf, "%d", min_unused_val);
	  val_to_write = min_unused_val;
	  status = add_to_dict_1 (fwd_ldb_dict, q_data_dir, keybuf, valbuf);
	  cBYE(status);
	  status = add_to_dict_1 (rev_ldb_dict, q_data_dir, valbuf, keybuf);
	  cBYE(status);
	  min_unused_val++;
	  if ( min_unused_val == INT_MAX ) { 
	    fprintf(stderr, "Cannot load more keys. Quitting \n"); go_BYE(-1);
	  }
	}
	else {
	  status = stoI4(valbuf, &val_to_write); cBYE(status);
	}
	if ( ofp != NULL ) { 
	  fwrite(&val_to_write, sizeof(int), 1, ofp);
	}
	switch ( num_cols ) { 
	  case 1 : state = READING_2nd_COL; break; 
	  case 2 : state = READING_1st_COL; break;
	  default : go_BYE(-1); break;
	}
	zero_string(keybuf,  MAX_LEN_STR); keyidx = 0;
	zero_string(joinbuf, MAX_LEN_STR); joinidx = 0;
	lno++; 
      }
      else {
	if ( keyidx > MAX_LEN_STR ) { go_BYE(-1); }
	keybuf[keyidx++] = (char)ic;
      }
      break;
    default : 
      go_BYE(-1);
      break;
    }
    if ( ( ( lno % 10000 ) == 0 ) && ( last_lno != lno ) ) {
      fprintf(stderr, "Processed %lld lines \n", lno);
      last_lno = lno;
    }
  }
  lno--;
  fprintf(stderr, "Processed %lld lines\n", lno);

 BYE:
  if ( ( infile != NULL ) && ( *infile != '\0' ) ) { 
    fclose_if_non_null(ifp);
  }
  fclose_if_non_null(ofp);
  fclose_if_non_null(jfp);
  if ( status < 0 ) { del_dict(ldb_dict, q_data_dir); }
  cleanUp();
  return status;
}
