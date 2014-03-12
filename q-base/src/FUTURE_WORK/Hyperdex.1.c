/*
While Hyperdex is a very powerful piece of software, our initial use
of it is quite modest.

We wish to use Hyperdex to create, store, edit, maintain and delete
dictionaries --- . I need to be able to invoke Hyperdex from within a
C program.

A dictionary is 1:1 mapping from strings to integers.

It would be very nice if the numbers that Hyperdex assigns to the
strings were 1, 2, 3, ... but that is not necessary. Random positive
integers would work as well.

Please ignore lines that start with RS: 

*/

int
mk_dict(
	char *dictionary
)
{
  // RS: returns -1 if ( ( dictionary == NULL ) || ( *dictionary == '\0' ) ) 
  // returns 0 if succesful, negative number otherwise.
  // If a dictionary exists with that name, then return negative number
  // indicating failure
}

bool
is_dict(
	char *dictionary
)
{
  // RS: returns -1 if ( ( dictionary == NULL ) || ( *dictionary == '\0' ) ) 
  // returns true if dictionary exists; false otherwise
}

int
add_to_dict_1(
	char *dict,
	char *str,
	bool just_checking,
	long long *ptr_valI8
)
{
  if ( ( str == NULL ) || ( *str == '\0' ) ) { return(-1); }
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( string "str" exists in dictionary "dict" ) { 
    *ptr_valI8 = its unique integer value return(0); 
  }
  else {
    if ( just_checking ) {
      *ptr_val_I8 = -1; return 0;
    }
    else {
      add string "str" to dictionary "dict"
      *ptr_valI8 = its unique integer value return(0); 
    }
  }
}

int add_key_val(
	char *dict,
	long long key,
	char *val
)
{
}

int get_key_val(
	char *dict,
	long long key,
	char *val
)
{
}

int del_key_val(
	char *dict,
	long long key,
	char *val
)
{
}

int
add_to_dict_2(
	char *dict,
	char *str,
	long long valI8
)
{
  if ( ( str == NULL ) || ( *str == '\0' ) ) { return(-1); }
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( string "str" exists in dictionary "dict" ) { 
    if ( its unique integer value == valI8 ) { 
      return(0);
    }
    else {
      return(-1);
    }
  }
  else {
    add string "str" to dictionary "dict" with unique integer value = valI8
  }
}

int
del_dict(
	char *dict
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary with name "dict" exists ) { 
    delete it;  return(0);
  }
  else {
    return(-1);
  }
}

int
del_from_dict(
	char *dict,
	char *str, /* Must specify str or valI8 but not both */
	long long valI8
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( ( str == NULL ) || ( *str == '\0' ) ) {
    if ( valI8 <= 0 ) { 
      return -1; 
    }
    else {
      if ( string "str" exists in dictionary "dict" ) { 
        delete it and return 0;
      }
      else {
        return(-1);
      }
    }
  }
  else {
    if ( valI8 > 0 ) {
      return(-1);
    }
    else {
      if ( string with integer value "valI8" exists in dictionary "dict" ) { 
        delete it and return 0;
      }
      else {
        return(-1);
      }
    }
  }
}
//--------------------------------------
int
get_from_dict_1( /* memory allocated by caller */
	char *dict,
	long long valI8
	char *output_str,
	int sz_output_str
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( valI8 <= 0 ) { return(-1); }
  if ( output_str == NULL ) { return(-1); }
  if ( sz_output_str <= 1 ) { return(-1); }
  if ( string with integer value "valI8" exists in dictionary "dict" ) { 
    Assume it has length len
    Copy up to sz_output_str-1 bytes to output_str
    Make sure output_str is null terminated
    if ( len >= sz_output_str ) {
      return -1;
    }
    else {
      return(0);
    }
  }
  else {
    *output_str = '\0';
    return(-1);
  }
}
//--------------------------------------
int
get_from_dict_2( /* memory allocated by callee */
	char *dict,
	long long valI8
	char **ptr_output_str
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( valI8 <= 0 ) { return(-1); }
  if ( output_str == NULL ) { return(-1); }
  if ( sz_output_str <= 1 ) { return(-1); }
  if ( string with integer value "valI8" exists in dictionary "dict" ) { 
    strdup(ptr_output_str, str); return(0); 
  }
  else {
    return(-1);
  }
}
