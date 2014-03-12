#include "qtypes.h"
#include "dictionary.h"

extern char *g_data_dir;
// START FUNC DECL
int
is_dictionary(
	      const char *dictionary,
	      bool *ptr_is_dict
	      )
// STOP FUNC DECL
{
  int status = 0;
  char cwd[MAX_LEN_DIR_NAME+1]; bool is_cd;
  FILE *fp = NULL;
  
  mcr_cd;
  *ptr_is_dict = file_exists(dictionary);
  mcr_uncd;
BYE:
  return(status);
}

  strcpy(dictPath, data_dir);
  /* Make sure '/' is at the end of directory path */
  if(dictPath[strlen(dictPath)-1] != '/'){
    strcat(dictPath, "/");
  }
  strcat(dictPath, dictionary);
  /* Now we have the path to dictionary */

  /* Folder based checking? */
  fp = fopen(dictPath, "r");
  if ( fp == NULL ) {
    method_1_exists = false;
  }
  else {
    method_1_exists = true;
    fclose_if_non_null(fp);
  }

  /* Second method of checking */
  LDBDictionaryPool * item = (LDBDictionaryPool *)ldbDictionaryPoolHead;
  if(!item){ /* head points to null */
    method_2_exists = false;
    status = _searchConflictStatus(dictPath, ptr_dict_id);
    cBYE(status);
    if ( *ptr_dict_id < DICTIONARY_ID_SEED ) { 
      method_2_exists = false;
    }
    else {
      method_2_exists = true;
    }
  }
  else {
    while(item && strcmp(item->dictionary->dictionaryPath, dictPath)){
      /* Skip to next */
      item = item->next;
    }
    if(!item){
      status = _searchConflictStatus(dictPath, ptr_dict_id);
      cBYE(status);
      if ( *ptr_dict_id < DICTIONARY_ID_SEED ) { 
        method_2_exists = false;
      }
      else {
	method_2_exists = true;
      }
    }
    else {
      method_2_exists = true;
      *ptr_dict_id = item->dictionary->dictionaryID;
    }
  }
  if ( method_1_exists != method_2_exists ) { go_BYE(-1); }

 BYE:
  return(status);
} /* is_dictionary */


/**
 * Deletes a dictionary and removes from disk
 */
int
delete_dictionary(
		  const char *data_dir,
		  const char *dictionary,
		  int dict_id
		  )
{
  int status = 0;
  char dictPath[PATHLEN+1];
  char cmd[100];
  dictionaryID_t did;

  strcpy(dictPath, data_dir);
  /* Make sure '/' is at the end of directory path */
  if(dictPath[strlen(dictPath)-1] != '/'){
    strcat(dictPath, "/");
  }
  strcat(dictPath, dictionary);
  
  LDBDictionaryPool * item = (LDBDictionaryPool *)ldbDictionaryPoolHead;

  /* Make sure the dictionary exists */
  status = is_dictionary(data_dir, dictionary, &did);

  if(status == 0){
    /* Dictionary exists, either in current or old list */

    while(item && item->dictionary->dictionaryID != dict_id){
      /* Skip to next */
      item = item->next;
    }
  
    /* At end?? */
    if(!item){
      /**
       * Not found in current list, 
       * Delete from old list 
       */
      deleteOldDictionary(dict_id);
    }
  
    else{      
      deleteDictionary(dict_id);
    }

    /* Delete the data on disk */
    sprintf(cmd, "%s %s %s", "rm", "-rf", dictPath);
    /* printf("CMD: %s\n", cmd); */
    system(cmd);
    /* TODO: Try delete_directory instead */
    status = 0;
  }
  else{
    status = -1;
  }
 BYE:
  return(status);
} /* delete_dictionary */

int
make_dictionary(
		const char *data_dir,
		const char *dictionary,
		int *ptr_dict_id
		)
{
  int status = 0;
  int dictID;
  
  status = is_dictionary(data_dir, dictionary, &dictID);
  cBYE(status);
  if ( dictID >= DICTIONARY_ID_SEED ) {
    /* Exists, delete */
    status = delete_dictionary(data_dir, dictionary, dictID);
    cBYE(status);
  }

  /* Now create one */


  /********* Be careful about the path! ********/
  /**
   * Because the API asks for a name that is created in the
   * Q_DATA_DIR directory
   */
  *ptr_dict_id = createDictionary(data_dir, dictionary, NULL, NULL);
  
  if(*ptr_dict_id < DICTIONARY_ID_SEED){
    /* Failure */
    status = -1;
  }
    
 BYE:
  return(status);
} /* make_dictionary */



int
add_to_dictionary(
		  const char *data_dir,
		  const char *dictionary,
		  int dict_id,
		  char *in_string,
		  unsigned long long *ptr_out_return_val
		  )
{
  int status = 0;
  char statCode;
  *ptr_out_return_val = (unsigned long long) addToDictionary(dict_id, in_string, &statCode);
  if(statCode == ERRORCODE){
    status = -1;
    *ptr_out_return_val = -1;
  }
  
 BYE:
  return(status);
} /* add_to_dictionary */

int
get_hash_from_dictionary(
			 const int dictionary_id,
			 char * const str,
			 int in_mode,
			 unsigned long long * ptr_hash,
			 bool * added
			 )
{
  int status = 0;
  char statCode, addStatus;
  Access_mode_t mode;
  
  switch ( in_mode ) { 
    case DICT_READ_MODE_SAFE  : mode = SAFE; break; 
    case DICT_READ_MODE_REUSE : mode = REUSE; break; 
    case DICT_READ_MODE_QUICK : mode = QUICK; break; 
    default : go_BYE(-1); break;
  }
  *ptr_hash = getHashFromDict(dictionary_id, str, &statCode, mode, &addStatus);
  if(statCode == ERRORCODE){
    status = -1;
  }
  else if(*ptr_hash == -1){
    status = -1;
  }
  
  if(mode == SAFE){
    if(addStatus == 1){
      *added = true;
    }
    else{
      *added = false;
    }
  }

 BYE:
  return(status);
}

int
get_from_dictionary(
		    const int dictionary_id,
		    const uint64_t hash,
		    char * buffer,
		    int buflen,
		    int * rslt_sz
		    )
{
  int status = 0;
  strcpy(buffer, getFromDictionaryWithSize(dictionary_id, hash, buflen, rslt_sz));
  if(*rslt_sz > buflen){
    status = 0;
  }
 BYE:
  return(status);
} /* get_from_dictionary */


/**
 * Returns the dictionary size 
 */
int
n_dictionary(
	     const int dictID,
	     long long * ptr_dict_size
	     /* char * dir, */
	     /* char * dictionary */
	     )
{
  int status = 0; 
  
  status = getDictSize(dictID, ptr_dict_size);

 BYE:
  return(status);
} /* n_dictionary */


int
list_dicts(
	   )
{
  int status = 0;
  
  status = listDicts();
  
 BYE:
  return(status);
}
