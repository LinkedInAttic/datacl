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
  /* Testing add_to_dict_4(), get_from_dict_4(), del_from_dict_4(), get_first_key_val_from_dict_4(), get_next_key_val_from_dict_4() and add_batch_to_dict_4() functions */


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

  int key[15]={0,0,4,2,3,1,2,4,2,3,0,2,3,2,0};
  int val[15]={1,2,1,2,1,2,1,2,1,2,1,2,1,2,1};

  /* In this section, trying to find out how many times each of the elements in key occured -- count_val */

  for ( int ii = 0; ii < 15; ii++ ) {

    int value;
    int key_exists;

    get_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], &value, & key_exists);

    /* if exists, increment value (incrementing count): value keeps track of count. else add with value 1 */

    if ( key_exists == 1 ) 
    {
      value++;
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], value);
    }
    else 
    {
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], 1);
    }

  }

  del_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, 1);
  del_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, 3);\
  /* 0, 2 and 4 will have their counts doubled */

  for ( int ii = 0; ii < 15; ii++ ) {

    int value;
    int key_exists;

    get_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], &value, & key_exists);

    /* if exists, increment value (incrementing count): value keeps track of count. else add with value 1 */

    if ( key_exists == 1 ) 
    {
      value++;
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], value);
    }
    else 
    {
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key[ii], 1);
    }

  }

  int current_key; 
 
  int next_exists;
  int first_exists;

  int temp_key;
  int temp_val;

  /* printing all key value pairs currently in the dictionary  */

  status = get_first_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, &temp_key, &temp_val,&first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%d,%d)\n", temp_key, temp_val);
    current_key = temp_key;
  }
  else { go_BYE(-1)};

  while(1) {

    status = get_next_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, current_key, &temp_key, &temp_val, &next_exists);
    cBYE(status);

    if  ( next_exists == 1 ) {
      printf("(%d,%d) \n", temp_key, temp_val);  
      current_key = temp_key;
    }
    else
      break;

  } 


  //--------------------------------------------------------------------


  status = add_batch_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, key ,val,15);
  cBYE(status);

  printf("\n\n");

  status = get_first_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, &temp_key, &temp_val,&first_exists);
  cBYE(status);

  if ( first_exists == 1 ) {
    /* there is atleast one key in the dictionary */
    printf("(%d,%d)\n", temp_key, temp_val);
    current_key = temp_key;
  }
  else { go_BYE(-1)};

  /*Note that only the values corresponding to the latest occurence of key will be stored in the dictionary (due to overwriting). refer to documentation in QLevelDB.h */

 /* printing all key value pairs now in the dictionary */

  while(1) {

    status = get_next_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, current_key, &temp_key, &temp_val, &next_exists);
    cBYE(status);

    if  ( next_exists == 1 ) {
      printf("(%d,%d) \n", temp_key, temp_val);  
      current_key = temp_key;
    }
    else
      break;

  } 

  //----------------------------------------------------------------------------------



 BYE:		

  cleanUp();
  return (status);
}
