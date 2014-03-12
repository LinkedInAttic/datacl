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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define OUTPUT_FILE_NAME "../outdata.bin"
#define KEY_FILE_NAME_1  "../key1.bin"
#define KEY_FILE_NAME_2  "../key2.bin"
#define LEN_FILE_NAME    "../len.bin"
#define OFFSET_FILE_NAME "../offset.bin"


void 
printPoll (
	   long long location
	   )
{

  if ( location % 1000000 == 0 && location > 0 ) {
    printf(".");
    fflush(stdout);
  }
  if ( location % 10000000 == 0 && location > 0 ) { 
    printf("%d", (int)location/1000000);
    fflush(stdout);
  }

}

int main (
	  int argc,
	  char ** argv
	  )
{
  
  int status = 0;

  if ( argc != 2 ) { assert(0); } 

  char * keyBuffer = NULL; 

  FILE * infile, * outfile, * keyfile1, * keyfile2, * offsetfile, * lenfile;

  infile = fopen  (argv[1], "r"); 
  outfile = fopen(OUTPUT_FILE_NAME, "wb"); 
  keyfile1 = fopen(KEY_FILE_NAME_1, "wb"); 
  keyfile2 = fopen(KEY_FILE_NAME_2,"wb"); 
  offsetfile = fopen(OFFSET_FILE_NAME,"wb");
  lenfile = fopen(LEN_FILE_NAME,"wb");

  if ( infile == NULL ) { assert(0); }

  /* infile: file with input string key information. example: {abc,def,ghij,abc,abc,xyz} 
     outfile: binary file containing the corresponding integer version of the input key information. example: {0,1,2,0,0,3} 
     keyfile1: assume that each input string is of constant length LEN (apprend nulls to ensure this), this file contains the strings corresponding to the integers. example:  {abc\0..\0,def\0..\0,ghij..\0,xyz\0..\0}, each being appended to make their length some fixed max_length. NOTE: we do this to make the process of finding what's the string corresponding to an integer easy, all we have to do is offset by max_length*integer.
     keyfile2, offsetfile, lenfile: if the input strings have varying lengths, instead of appending zero's to make them same length we just write them and keep track of the necessary offset of be done in keyfile2 to identify the string corresponding to integer i in offsetfile. lenfile keeps track of length of the i'th string */


  /* Setup the dictionary. If another dictionary of the same name exists, delete that dictionary and create a new one */
  
  int dict_exists;
  status = is_dict (TEST_DB_NAME, TEST_DIR_NAME, & dict_exists);
  cBYE(status);

  if ( dict_exists == 1 ) {

    printf("Deleting existing dictionary\n");
    status = del_dict(TEST_DB_NAME, TEST_DIR_NAME);
    cBYE(status);

  }

  printf("Creating new dictionary\n");
  status = mk_dict (TEST_DB_NAME, TEST_DIR_NAME); 
  cBYE(status);
 

  size_t len = MAX_STR_LEN;
  ssize_t read;
  keyBuffer = malloc ( MAX_STR_LEN * sizeof(char) );
  
  int min_unused_value = 1;
  long long count_iter = 0;
  
  int offSet = 0;

  /* Scan through all the keys in the input data. 
     For each key, check if the key already exists in the levelDB dictionary.
     (i) Yes: get it's value and write the value to the output binary file. 
     (ii) No: assign it an unused value and write the value to the output. Also update keyfile. */

  printf("Processing input data\n");
  while ( (read = getline (&keyBuffer, &len, infile)) != -1 ) {

    int value = -1;
    int keyLength = strlen(keyBuffer);
    int keyExists = 0;
    keyBuffer[keyLength-1]='\0'; /* replaces \n with \0 */

    status = get_from_dict_2 (TEST_DB_NAME, TEST_DIR_NAME, keyBuffer, &value, &keyExists);
    cBYE(status);

    if ( keyExists == 0 ) {

      status = add_to_dict_2 (TEST_DB_NAME, TEST_DIR_NAME, keyBuffer, min_unused_value);
      cBYE(status);
      
      fwrite (&min_unused_value, sizeof(int), 1, outfile);
      min_unused_value++;
      
      /* update key file */

      fwrite (keyBuffer, MAX_STR_LEN*sizeof(char), 1, keyfile1);
      fwrite (keyBuffer, keyLength*sizeof(char), 1, keyfile2);
      fwrite (&keyLength, sizeof(int),1,lenfile);
      fwrite (&offSet, sizeof(int),1,offsetfile);
    
      offSet += keyLength;

    }
    else {
      fwrite (&value, sizeof(int), 1, outfile);
    }

    printPoll(count_iter++);
 
  }
  printf("\nProcess completed\n");
  

 BYE:

  fclose (infile);
  fclose (outfile);
  fclose (keyfile1);
  fclose (keyfile2);
  fclose (offsetfile);
  fclose (lenfile);

  free_if_non_null (keyBuffer);

  return (status);

}
