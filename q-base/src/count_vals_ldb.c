#include "QLevelDB.h"
#include <stdio.h>
#include <time.h>
#include "constants.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "macros.h"
#include "count_vals_ldb.h"

#define TEST_DB_NAME "testDB"
#define TEST_DIR_NAME "/tmp/"

// START FUNC DECL
int count_vals_ldb (
		    int * x,
	      	    char * cfld,
		    long long siz,
		    FILE * idfp,
		    FILE * freqfp,
		    long long * out_siz
		    )
// STOP FUNC DECL
//-----------------------------------------------------------------------

/* README: 

count_vals_ldb: takes as input an array of integers and outputs them in the form of (id, freq), i.e, which are the elements in the set and how many times they occur. The set of id's will be written in the binary format to the file pointed by idfp and the set with their corresponding frequencies will be written in the binary format to the file pointed by freqfp. The sets will be sorted in id.

NOTE: LevelDB will be used to do the calculations.

For ex: take the input set {1,2,3,1,2,2,2,2,1,1}. ids are {1,2,3} (sorted) and their corresponding frequencies are {4,5,1}. 


INPUTS: 

x: the array containing the input elements 

cfld: character array acting as select operator for x. Two modes:
(i) NULL: all elements of x will be processed
(i) non-NULL: only those elements with non-zero cfld entries will be processed.

siz: number of elements in the input set

OUTPUTS:

idfp: binary file pointer, the ids in the input set will be written here in sorted order

freqfp: binary file pointer, the corresponding frequencies of the ids will be written here

outsiz: number of elements in the output binary file ( same as the number of distinct elements in the input set)

*/

//-----------------------------------------------------------------------
{

  int status = 0;

  /* Check inputs */
  if ( x == NULL ) { go_BYE(-1); }
  if ( idfp == NULL ) { go_BYE(-1); }
  if ( freqfp == NULL ) { go_BYE(-1); }
  if ( siz <= 0 ) { go_BYE(-1); }


  /* Check if a LevelDB dictionary with the same name and pathname exists. If yes, delete it and create a new one */

  int dict_exists;
  status = is_dict(TEST_DB_NAME, TEST_DIR_NAME, &dict_exists);
  cBYE(status);

  if ( dict_exists == 1 ) { 
    status = del_dict(TEST_DB_NAME, TEST_DIR_NAME); cBYE(status);
  }

  status = mk_dict(TEST_DB_NAME, TEST_DIR_NAME);
  cBYE(status);

  //----------------------------------------------------------------------

  /* Algorithm: The id's will be stored as keys in the dictionary, their corresponding values will be the number of times they have occured till then. 

     (i)  Check if the id exists in the dictionary ( using get_from_dict() )
     (ii) If yes, increment it's value by 1 and add it to the dictionary ( leveldb overwrites values, so just adding the key using add_to_dict() with the new value is sufficient )
     (iii)If no, add the key with a value 1 using add_to_dict() 

     NOTE: We will be using add_to_dict_4() and get_from_dict_4() since in this problem, both keys and values are integers

     Refer to "QLevelDB.h" for details. */

  for ( long long ii = 0; ii < siz; ii++ ) {
 
    if ( cfld == NULL ) {}
    else if ( cfld[ii] == 0 ) { continue; }
    
    int value;
    int key_exists;

    status = get_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, x[ii], &value, &key_exists); cBYE(status);

    if ( key_exists == 1 ) {
      value++;
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, x[ii], value);
    }
    else {
      value = 1;
      add_to_dict_4(TEST_DB_NAME, TEST_DIR_NAME, x[ii], value);
    }

  }

  //----------------------------------------------------------------------

  /* Write out all the keys and values sequentially to their respective files. We use get_next_key_val_from_dict_4() to do this. We initialize current_key to a value lesser than all the keys we have seen so that the iteration will start from first.*/

  long count_iter = 0;

  int current_key, temp_key;
  int temp_val;
  int first_exists, next_exists;
  
 /* variables for get_first_key_val_from_dict_4() and get_next_key_val_from_dict_4() */

  status = get_first_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, &temp_key, &temp_val, &first_exists);
  cBYE(status);
  
  if ( first_exists == 1 ) {
     fwrite(&temp_key, sizeof(int), 1, idfp); 
     fwrite(&temp_val, sizeof(int), 1, freqfp);
     current_key = temp_key;
     count_iter++;
  }
  else { 
    *out_siz = 0; 
    go_BYE(-1);
  }
  

  while ( 1 )  {

    status = get_next_key_val_from_dict_4(TEST_DB_NAME, TEST_DIR_NAME, current_key, &temp_key, &temp_val, &next_exists);
    cBYE(status);
    
    if ( next_exists == 1 ) {
      fwrite(&temp_key, sizeof(int), 1, idfp); 
      fwrite(&temp_val, sizeof(int), 1, freqfp);
      current_key = temp_key;
      count_iter++;
    }
    else
      break;

  } 

  *out_siz = count_iter;

  
 BYE:

  cleanUp();
  return (status);

}



