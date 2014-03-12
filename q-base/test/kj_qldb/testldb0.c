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

#define TEST_DB_NAME "testDB"
#define TEST_DIR_NAME "/tmp/"

int main ()
{

  int status = 0;

  //-----------------------------------------------------------------
  /* Testing  mk_dict(), is_dict() and del_dict() functions */

  int dict_exists;

  printf("\n");

  status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("Deleted successfully\n");

  status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("Deleted successfully\n");

  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);
  printf("is :%d\n", dict_exists); // Should be 0, since we deleted it

  status = mk_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("Created successfully\n");

  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);
  printf("is :%d\n", dict_exists); // Should be 1, since we created it

  status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("Deleted successfully\n");

  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);
  printf("is :%d\n", dict_exists); // Should be 0, since we created it

  status = mk_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);
  printf("Created successfully\n");
  
  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);
  printf("is :%d\n", dict_exists); // Should be 1, since we created it

  //-----------------------------------------------------------------


 BYE:

  cleanUp();
  return status;
}
