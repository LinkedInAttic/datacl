//-------------------------------------------------------------------------

/* Functions for using LevelDB (key, value) storage from C codes. 4 formats are supported: 
(i) both key and value are strings
(ii) key is a string and value is a 4-byte integer
(iii) key is a 4-byte integer and value is a string
(iv) both key and value are 4-byte integers */

//------------------------------------------------------------------------
/* Functions to create, delete and check if a dictionary with the specified name already exists at the specified pathname. 

NOTE: LevelDB does not have the ability to create the folders necessary for the pathname to make sense. Ensure that the pathname is valid by MANUALLY creating the necessary folders */
//------------------------------------------------------------------------

int mk_dict (
	     const char * name, 
	     const char * pathname
	     );
/* mk_dict(): 
makes a LevelDB dictionary with the specified pathname and name.  Return values 0 or -1:

0: Dictionary was created successfully 
-1: Dictionary creation failed, something fundamentally wrong. Most likely reason for this: folders mentioned in the path name don't exist (create them manually) */

int del_dict (
	      const char * name,
	      const char * pathname
	      );
/* del_dict(): 
removes the LevelDB dictionary with the specified pathname and name. Return values 0 or -1:

0: Specified dictionary, if existed, was removed successfully. If it did not exist to begin with, 0 will be returned.
-1: Something fundamentally wrong */

int is_dict (
	     const char * name, 
	     const char * pathname,
	     int * dict_exists
	     );
/* is_dict(): 
checks if the LevelDB dictionary with the specified pathname and name already exists. Return values 0 or -1:

0: Check was done properly. In this case, *dict_exists takes values 1 or 0.
-1: Something fundamentally wrong 

dict_exists: pointer storing values 0 or 1:
0: a dictionary with the same name and pathname doesn't exist
1: a dictionary with the same name and pathname already exists */

void cleanUp();
/* before closing the program, just call this function to close active database connections ( acts just like fclose ) */

//------------------------------------------------------------------------
/* Functions for (string, string) LevelDB dictionaries, i.e., both keys and values are strings */
//------------------------------------------------------------------------

int add_to_dict_1 (
		 const char * name, 
		 const char * pathname, 
		 const char * key,
		 const char * value
		 );
/* add_to_dict_1():
adds the specified (key, value) pair to the specified dictionary. If the key already exists, it's value will be replaced by the new value. Return values: 0 or -1:

0: addition was done successfully
-1: something fundamentally wrong. Probable reasons: (i) dictionary doesn't exist (ii) "key" or "value" is NULL, NULLs cannot be handled unfortunately */

int get_from_dict_1 (
		   const char * name, 
		   const char * pathname, 
		   const char * key,
		   char * value,
		   int value_size,
		   int * key_exists
		   );
/* get_from_dict_1():
gets the value corresponding to the specified key from the specified dictionary. If the key exists, it's value will be stored in the location pointed by "value" (pre-allocated with "value_size" amount of memory) and *key_exists will be set to 1. Else, *key_exists will be set to 0. Return values: 0 or -1:

0: Search was done successfully, refer to key_exists to understand results
-1: Something fundamentally wrong. Probable reasons: (i) dictionary doesn't exist (ii) "key" is NULL (iii) sufficient memory was not pre-allocated to "value"

NOTE: "value" has to be pre-malloced so that writing can be done there by LevelDB, I suggest pre-allocing by MAX_VAL_LEN, defined in "constants.h". 

value_size: size of the memory pre-allocated to "value"

key_exists: pointer storing 0 or 1:
0: the specified key doesn't exist in the dictionary
1: the specified key already exists in the dictionary. */

int del_from_dict_1 (
		   const char * name, 
		   const char * pathname, 
		   const char * key
		   );
/*del_from_dict_1():
deletes the (key, value) pair corresponding to the specified key from the specified dictionary. Return values: 0 or -1:

0: deleting process completed succesfully. Even if the key doesn't exist, 0 will be returned. 
-1: something fundamentally wrong. Probable reasons (i) dictionary doesn't exist (ii) "key" is NULL */

int get_first_key_val_from_dict_1 (
				const char * name, 
				const char * pathname, 
				char * key,
				const int key_size,
				char * value,
				const int value_size,
				int * first_exists
				);
/* get_first_key_val_from_dict_1(): 
the function will write the first key (sorted lexicographically) and it's corresponding value from the specified dictionary to "key" and "value" buffers, which were preallocated with "key_size" and "value_size" amount of memory. If there was no key in the dictionary, it'll set *first_exists to 0, else set it to 1. Return values:
0: The process was completed successfully, see *first_exists to interpret results
-1: Somthing fundamentally wrong. Probable reasons: (i) dictionary doesn't exist (ii) Sufficient memory not allocated to "key" or "value"

first_exists: pointer to location having 0 or 1
0: the dictionary has no keys, nothing was done to "key" or "value"
1: the first key value pair were written to "key" and "value" (both of them preallocated with sufficient memory) respectively.

NOTE: preallocate memory to "key" and "value", can use MAX_KEY_LEN and MAX_VAL_LEN (defined in constants.h) if unsure

*/

int get_next_key_val_from_dict_1 (
				const char * name, 
				const char * pathname, 
				char * current_key,
				char * key,
				const int key_size,
				char * value,
				const int value_size,
				int * next_exists
				);
/*get_next_key_val_from_dict_1():
for a specified "current_key", the function will return the next key (in the lexicographic order) which exists in the LevelDB dictionary and it's corresponding value and store it in the location pointed by "key" and "value" respectively (both of them pre-alloced with "key_size" and "value_size" amount of memory, also sets *next_exists to 1 in the process. If there is no next key in the dictionary for the specified "current_key", *next_exists will be set to 0. Return values: 

0: The process was completed successfully, see *next_exists to interpret the results.
-1: Something fundamentally wrong. Probable reasons: (i) dictionary doesn't exist (ii) current_key might be NULL (iii) sufficient memory not allocated to "key" or "value"

NOTE: This function is particularly useful to scan through a dictionary and get all the (key, value) pairs one by one. It can be done as follows: get the first key using get_first_key_val_from_dict_1() and call this function using first key as the current key, it should return the second key. Use this key and rerun the function to get the third (key,value) pair and so on. Stop when *next_exists is 0, you have reached the end.

EXAMPLE: say the dictionary has {bala, bharath, kishore, ramesh} as keys.Using get_first_key_val_from_dict_1() we get bala as the first key, setting current_key to bala will give bharath as the next key. Setting current key to "dev" will give kishore as the next key (lexicographic). setting current key to "zebra" will not give any key output and hence set next_exists to 0.

NOTE: "key" and "value" have to be pre-allocated with memory. I suggest using MAX_KEY_LEN and MAX_VAL_LEN, defined in "constants.h".

next_exists: pointer storing 0 or 1:
0: you have reached the end of the dictionary, i.e, there are no keys in the dictionary which come later than the specified current_key in the lexicographical order
1: there were still some keys in the dictionary which come later than the specified current_key in the lexicographical order. The next one was stored in "key" and "value". */

int add_batch_to_dict_1 (
		       const char * name, 
		       const char * pathname, 
		       char ** batch_key,
		       char ** batch_value,
		       int batch_size
		       ); 
/*add_batch_to_dict_1():
this function adds a batch of (key, value) pairs to the LevelDB dictionary, specifically "batch_size" number of (key, value) pairs will be added to the dictionary. Functionally, this is the same as calling add_to_dict_1() "batch_size" number of times but it is super-efficient. The batch of key data and value data should be passed using 2d pointers, so that the function can refer to batch_key[ii] and batch_value[ii] to get the ii'th (key, value) pair respectively (where ii ranges from 0 to batch_size-1).

Return values:
0: addition was done succesfully
-1: something fundamentally wrong. Probable reasons (i) dictionary doesn't exist (ii) some "key" or "value" has NULL 

NOTE: recommend using it when:
(i) all keys are unique
(ii) if keys in a (key, value) input occur more than once, the value corresponding to only the last occurence matters and everything else can be ignored
for example: (hello, world) (hi, there) (hello, universe) is the incoming data say. "hello" will have a value of "universe" since it corresponds to the last occurence of "hello". 

NOTE: cannot be used for operations like counting how many times a key has occured, since you need to keep track of all the occurences.  */

//------------------------------------------------------------------------
/* Functions for (string, int) LevelDB dictionaries, i.e., key is a string and value is a 4-byte integer */
//------------------------------------------------------------------------

int add_to_dict_2 (
		   const char * name, 
		   const char * pathname, 
		   const char * key,
		   const int value
		   );
/* refer to add_to_dict_1() */

int get_from_dict_2 (
		     const char * name, 
		     const char * pathname, 
		     const char * key,
		     int * value,
		     int * key_exists
		     );
/* refer to get_from_dict_1() */

int del_from_dict_2 (
		     const char * name, 
		     const char * pathname, 
		     const char * key
		     );
/* refer to del_from_dict_1() */

int get_first_key_val_from_dict_2 (
				  const char * name, 
				  const char * pathname, 
				  char * key,
				  const int key_size,
				  int * value,
				  int * first_exists
				  );
/* refer to get_first_key_val_from_dict_1() */

int get_next_key_val_from_dict_2 (
				  const char * name, 
				  const char * pathname, 
				  char * current_key,
				  char * key,
				  const int key_size,
				  int * value,
				  int * next_exists
				  );
/* refer to get_next_key_val_from_dict_1() */

int add_batch_to_dict_2 (
		       const char * name, 
		       const char * pathname, 
		       char ** batch_key,
		       int * batch_value,
		       int batch_size
		       ); 
/* refer to add_batch_to_dict_1(). Note that batch_value is now an integer array, so that batch_value[ii] will refer to the value corresponding to the ii'th key (which will be stored in batch_key[ii] string), where ii ranges from 0 to batch_size-1 */

//------------------------------------------------------------------------
/* Functions for (int, string) LevelDB dictionaries, i.e., key is a 4-byte integer and value is a string */
//------------------------------------------------------------------------

int add_to_dict_3 (
		   const char * name, 
		   const char * pathname, 
		   const int key,
		   char * value
		   ); 
/* refer to add_to_dict_1() */

int get_from_dict_3 (
		     const char * name, 
		     const char * pathname, 
		     const int key,
		     char * value,
		     int value_size,
		     int * key_exists
		     ); 
/* refer to get_from_dict_1() */

int del_from_dict_3 (
		     const char * name, 
		     const char * pathname, 
		     const int key
		     );
/* refer to del_from_dict_1() */

int get_first_key_val_from_dict_3 (
				  const char * name, 
				  const char * pathname, 
				  int * key,
				  char * value,
				  const int value_size,
				  int * first_exists
				  );
/* refer to get_first_key_val_from_dict_1(). NOTE: Since leveldb always stores in strings, the lexicographic order after using sprintf() will be the comparator function. For ex: {1,10,11,12,2,20,22} is the sorted order etc. */

int get_next_key_val_from_dict_3 (
				  const char * name, 
				  const char * pathname, 
				  int current_key,
				  int * key,
				  char * value,
				  const int value_size,
				  int * next_exists
				  );
/* refer to get_next_key_val_from_dict_1(). NOTE: Since leveldb always stores in strings, the lexicographic order after using sprintf() will be the comparator function. For ex: {1,10,11,12,2,20,22} is the sorted order etc. */

int add_batch_to_dict_3 (
		       const char * name, 
		       const char * pathname, 
		       int * batch_key,
		       char ** batch_value,
		       int batch_size
		       ); 
/* refer to add_batch_to_dict_1(). Note that batch_key is now an integer array, so that batch_key[ii] will refer to the the ii'th key (whose corresponding value will be stored in batch_value[ii] string) , where ii ranges from 0 to batch_size-1 */

//------------------------------------------------------------------------
/* Functions for (int, int) LevelDB dictionaries, i.e., both keys and values are integers */
//------------------------------------------------------------------------

int add_to_dict_4 (
		   const char * name,
		   const char * pathname,
		   const int key,
		   const int value
		   );
/* refer to add_to_dict_1() */

int get_from_dict_4 (
		     const char * name,
		     const char * pathname,
		     const int key,
		     int * value,
		     int * key_exists
		     );
/* refer to get_from_dict_1() */

int del_from_dict_4 (
		     const char * name, 
		     const char * pathname, 
		     const int key
		     );
/* refer to del_from_dict_1() */

int get_first_key_val_from_dict_4 (
				  const char * name, 
				  const char * pathname, 
				  int * key,
				  int * value,
				  int * first_exists
				  );
/* refer to get_first_key_val_from_dict_1() and get_first_key_val_from_dict_3() */

int get_next_key_val_from_dict_4 (
				  const char * name, 
				  const char * pathname, 
				  int current_key,
				  int * key,
				  int * value,
				  int * next_exists
				  );
/* refer to get_next_key_val_from_dict_1() and get_next_key_val_from_dict_3() */

int add_batch_to_dict_4 (
		       const char * name, 
		       const char * pathname, 
		       int * batch_key,
		       int * batch_value,
		       int batch_size
		       ); 
/* refer to add_batch_to_dict_1(). Note that both batch_key and batch_value are integer arrays now, so that (batch_key[ii], batch_val[ii]) refers to the ii'th (key, value) pair where ii ranges from 0 to batch_size-1 */

//-----------------------------------------------------------------------


