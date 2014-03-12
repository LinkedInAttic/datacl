/*
While Hyperdex is a very powerful piece of software, our initial use
of it is quite modest.

We wish to use Hyperdex to map from strings to unique integers. For
example, say that we create dictionary "D1".  When Hyperdex is
presented string "abc" for dictionary "D1", it will return an integer,
say i1. Then, every time dictionary "D1" is presented with the string
"abc", it will return i1. If dictionary "D1" is presented with a
string other than "abc", it must NOT return i1.

It would be very nice if the numbers that Hyperdex assigns to the
strings were 1, 2, 3, ... but that is not necessary. Random positive
8-byte integers would work as well.

*/

int
mk_dict(
	char *dict
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary exist with name "dict" ) { return(-1); }
  // Create dictionary with name "dict"
  return(0); 
}

int 
is_dict(
	char *dict,
	bool *ptr_is_dict
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary "dict" exists ) {
    *ptr_is_dict = true;
  }
  else {
    *ptr_is_dict = false;
  }
  return(0);
}

int
add_to_dict(
	char *dict,
	char *str,
	long long *ptr_valI8
)
{
  if ( ( str  == NULL ) || ( *str  == '\0' ) ) { return(-1); }
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary "dict" does not exit ) { return(-1); }
  if ( string "str" does not exists in dictionary "dict" ) { 
    add string "str" to dictionary "dict"
  }
  *ptr_valI8 = unique integer value assigned by Hyperdex;
  return(0); 
}

int
is_in_dict(
	char *dict,
	char *str,
	bool  *ptr_is_in_dict
)
{
  if ( ( str  == NULL ) || ( *str  == '\0' ) ) { return(-1); }
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary "dict" does not exit ) { return(-1); }
  if ( string "str" exists in dictionary "dict" ) { 
    *ptr_is_in_dict = true;
  }
  else {
    *ptr_is_in_dict = false;
  }
  return(0); 
}

int
del_dict(
	char *dict
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary "dict" exists ) { 
    delete it;  
    return(0);
  }
  else {
    return(-1);
  }
}

//--------------------------------------
int
get_from_dict( /* memory allocated by caller */
	char *dict,
	long long valI8
	char *output_str,
	int sz_output_str
)
{
  if ( ( dict == NULL ) || ( *dict == '\0' ) ) { return(-1); }
  if ( dictionary "dict" does not exit ) { return(-1); }
  if ( output_str == NULL ) { return(-1); }
  if ( sz_output_str <= 1 ) { return(-1); }

  if ( string "str" with integer value "valI8" exists in dictionary "dict" ) { 
    strncpy(output_str, str, sz_output_str-1);
    len = strlen(str); 
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
