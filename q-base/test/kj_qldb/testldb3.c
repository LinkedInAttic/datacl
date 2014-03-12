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
#include "constants.h"
#include "macros.h"

typedef char* string;

#define TEST_DB_NAME "testDB"
#define TEST_DIR_NAME "/tmp/"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int main ()
{
  int status = 0;

 //-----------------------------------------------------------------
  /* Testing add_to_dict_3(), get_from_dict_3(), del_from_dict_3(), get_first_key_val_from_dict_3(), get_next_key_val_from_dict_3() and add_batch_to_dict_3() functions */

  int dict_exists;

  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);
 
  if ( dict_exists == 1 ) {

    status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
    cBYE(status);
    printf("\n\nDeleting existing dictionary\n");

  }

  status = mk_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("\n\nCreating new dictionary\n\n");

  string str[10];
  
  str[0] = "kishore";
  str[1] = "ramesh";
  str[2] = "bharath";
  str[3] = "kishore";
  str[4] = "bala";
  str[5] = "bala";
  str[6] = "ramesh";
  str[7] = "kishore";
  str[8] = "ramesh";
  str[9] = "kaushik";

  int key[10] = {0,1,2,3,2,4,3,4,3,0};

  /* In this loop, we do the following operations: everytime we see a new unseen key, we assign it the corresponding value (string str has it now) (i.e., values corresponding to the first occurence of hte keys will be stored) */

  for ( int ii = 0; ii < 10; ii++ ) {

    int key_exists;
    char temp_str[10];

    status = get_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, key[ii] ,temp_str, MAX_STR_LEN, & key_exists);
    cBYE(status);

    if ( key_exists == 0  ) {
  
      /* key does not exist in the dictionary */
      printf("get: %d %d\n", key[ii], key_exists);
      
      status = add_to_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, key[ii], str[ii]);
      cBYE(status);

      printf("add: (%d,%s)\n", key[ii], str[ii]);
    }
    else {
      printf("get: (%d,%s) %d\n", key[ii], temp_str, key_exists);
    }
 
  }
  /* testing some deletes */
  status = del_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, 2);
  cBYE(status);
  printf("del: 2\n" );
  status = del_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, 4);
  cBYE(status); 
  printf("del: 4\n" );
  status = del_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, 6);
  cBYE(status);
  printf("del: 6\n" );
  status = del_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, 8);
  cBYE(status); 
  printf("del: 8\n" );
  status = del_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, 10);
  cBYE(status);

  /* In this loop, we redo the addition to see its effect */

  for ( int ii = 0; ii < 10; ii++ ) {

    int key_exists;
    char temp_str[10];

    status = get_from_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, key[ii] ,temp_str, MAX_STR_LEN, & key_exists);
    cBYE(status);

    if ( key_exists == 0  ) {

      /* key does not exist in the dictionary */
      printf("get: %d %d\n", key[ii], key_exists);
      
      status = add_to_dict_3 (TEST_DB_NAME, TEST_DIR_NAME, key[ii], str[ii]);
      cBYE(status);

      printf("add: (%d,%s)\n", key[ii], str[ii]);
    }
    else {
      printf("get: (%d,%s) %d\n", key[ii], temp_str, key_exists);
    }
 
  }


  //----------------------------------------------------------------------------------

  /* In this section we print all the (key, value) pairs currently in the dictionary sequentially */

  printf("\n\n");

  int temp_key;
  char temp_val[MAX_VAL_LEN];
  
  int current_key; 

  int next_exists;
  int first_exists;

  status = get_first_key_val_from_dict_3(TEST_DB_NAME, TEST_DIR_NAME, &temp_key, temp_val, MAX_VAL_LEN,&first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%d,%s)\n", temp_key, temp_val);
    current_key = temp_key;
  }
  else { go_BYE(-1)};
  
  while ( 1 )  {

    status = get_next_key_val_from_dict_3(TEST_DB_NAME, TEST_DIR_NAME, current_key, &temp_key,temp_val, MAX_VAL_LEN, &next_exists);
    cBYE(status);
    
    if ( next_exists == 1 ) {

      printf("(%d,%s)\n", temp_key, temp_val);
      current_key = temp_key;

    }
    else
      break;

  } 

  printf("\n\n");

  /* adding a batch of (key, value) pairs, all 10 at the same time. Note that only the values corresponding to the latest occurence of key will be stored in the dictionary (due to overwriting). refer to documentation in QLevelDB.h */

  add_batch_to_dict_3(TEST_DB_NAME, TEST_DIR_NAME, key, str, 10);

  status = get_first_key_val_from_dict_3(TEST_DB_NAME, TEST_DIR_NAME, &temp_key, temp_val, MAX_VAL_LEN,&first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%d,%s)\n", temp_key, temp_val);
    current_key = temp_key;
  }
  else { go_BYE(-1)};

  
  while ( 1 )  {

    status = get_next_key_val_from_dict_3(TEST_DB_NAME, TEST_DIR_NAME, current_key, &temp_key,temp_val, MAX_VAL_LEN, &next_exists);
    cBYE(status);
    
    if ( next_exists == 1 ) {

      printf("(%d,%s)\n", temp_key, temp_val);
      current_key = temp_key;

    }
    else
      break;

  } 



 BYE:						

  cleanUp();
  return (status);
}
