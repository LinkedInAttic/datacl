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

  char * current_key = NULL;

  //-----------------------------------------------------------------
  /* Testing add_to_dict_1(), get_from_dict_1(), del_from_dict_1(), get_first_key_val_from_dict_1(), get_next_key_val_from_dict_1() and add_batch_to_dict_1() functions */

  int dict_exists;

  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);

  if ( dict_exists == 1 ) {

    status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
    cBYE(status);
    printf("\nDeleting existing dictionary\n");

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

  string val[10]={"0","1","2","3","4","5","6","7","8","9"};

  int min_unused_val = 1;

  /* In this loop, we do the following operations: every time we see a new key, we assign it a new integer (which will be stored as it's corresponding value) */

  for ( int ii = 0; ii < 10; ii++ ) {

    char temp_str[MAX_STR_LEN];
    int key_exists;

    status = get_from_dict_1 (TEST_DB_NAME, TEST_DIR_NAME, str[ii],temp_str,MAX_STR_LEN, & key_exists);
    cBYE(status);

    if ( key_exists == 0  ) {
  
      /* the key does not exist in the dictionary, will be added to it with a new integer not used by anyone else */
      printf("get: %s %d\n",str[ii], key_exists);
      
      sprintf(temp_str,"%d", min_unused_val);
      status = add_to_dict_1 (TEST_DB_NAME, TEST_DIR_NAME, str[ii], temp_str);
      cBYE(status);

      printf("add: (%s,%s)\n", str[ii],temp_str);
      min_unused_val++;
    }
    else {
      printf("get: (%s,%s) %d\n", str[ii],temp_str, key_exists);
    }
 
  }

  /* deleting some keys from the dictionary, for testing purpose */
  for ( int ii = 0; ii < 2; ii++ ) { 

    status = del_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, str[ii]);
    cBYE(status);

    printf("del: %s\n", str[ii]);

  }
  status = del_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, "linkedin");
  cBYE(status);
  printf("del: %s\n", "linkedin");

  /* everytime we see a new key, we assign it a new integer ( which will be stored as its corresponding value. readding for testing purpose */

  for ( int ii = 0; ii < 10; ii++ ) {

    char temp_str[10];
    int key_exists;

    status = get_from_dict_1 (TEST_DB_NAME, TEST_DIR_NAME, str[ii],temp_str,MAX_STR_LEN, & key_exists);
    cBYE(status);

    if ( key_exists == 0 ) {
  
      /* key doesn't exist in the dictionary (the ones we deleted earlier) */
      printf("get: %s %d\n",str[ii], key_exists);
      
      sprintf(temp_str,"%d", min_unused_val);
      status = add_to_dict_1 (TEST_DB_NAME, TEST_DIR_NAME, str[ii], temp_str);
      cBYE(status);

      printf("add: (%s,%s)\n", str[ii],temp_str);
      min_unused_val++;
    }
    else {
      printf("get: (%s,%s) %d\n", str[ii],temp_str, key_exists);
    }
 
  }

  //--------------------------------------------------------------------------

  /* in this section, we print all the (key, value) pairs currently in the dictionary sequentially */

  printf("\n\n");

  char temp_key[MAX_KEY_LEN];
  char temp_val[MAX_VAL_LEN];
  
  current_key = malloc ( MAX_STR_LEN * sizeof(char) );

  int first_exists;
  int next_exists;

  status = get_first_key_val_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, temp_key, MAX_KEY_LEN, temp_val, MAX_VAL_LEN, &first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%s,%s)\n", temp_key, temp_val);
    strcpy(current_key,temp_key);
  }
  else { go_BYE(-1)};

  while ( 1 )  {

    status = get_next_key_val_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, current_key, temp_key, MAX_KEY_LEN,temp_val, MAX_VAL_LEN, &next_exists);
    cBYE(status);
    
    if ( next_exists == 1 ) {
      
      /* more new data found */
      printf("(%s,%s)\n", temp_key, temp_val);
      strcpy(current_key,temp_key);

    }
    else
    {
      /* reached end of dictionary */
      break;
    }

  } 

  printf("\n\n");

  /* adding a batch of (key, value) pairs, all 10 at the same time. Note that only the values corresponding to the latest occurence of key will be stored in the dictionary (due to overwriting). refer to documentation in QLevelDB.h */

  status = add_batch_to_dict_1(TEST_DB_NAME, TEST_DIR_NAME, str, val, 10);
  cBYE(status);
  /* printing all the (key, value) pairs now in the dictionary */

  status = get_first_key_val_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, temp_key, MAX_KEY_LEN, temp_val, MAX_VAL_LEN, &first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%s,%s)\n", temp_key, temp_val);
    strcpy(current_key,temp_key);
  }
  else { go_BYE(-1)};
  
  while ( 1 )  {

    status = get_next_key_val_from_dict_1(TEST_DB_NAME, TEST_DIR_NAME, current_key, temp_key, MAX_KEY_LEN,temp_val, MAX_VAL_LEN, &next_exists);
    cBYE(status);
    
    if ( next_exists == 1 ) {
      
      /* more new data found */
      printf("(%s,%s)\n", temp_key, temp_val);
      strcpy(current_key,temp_key);

    }
    else
    {
      /* reached end of dictionary */
      break;
    }

  } 


  //----------------------------------------------------------------------------------

 BYE:

  free_if_non_null ( current_key);
  cleanUp();

  return (status);
}
