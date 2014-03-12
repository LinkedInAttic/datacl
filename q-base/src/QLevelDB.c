#include "QLevelDB.h"
// DO NOT AUTO GENERATE HEADER FILE 
#include "DictFunctions.h"


//---------------------------------------------------------------------------

int mk_dict(const char * name, const char * pathname)
{
  return mk_dict_cc(name,pathname);
}

int del_dict(const char * name,const char * pathname)
{
  return del_dict_cc(name,pathname);
}

int is_dict(const char * name, const char * pathname, int * dict_exists)
{
  return is_dict_cc(name, pathname, dict_exists);
}

//---------------------------------------------------------------------------

int add_to_dict_1(const char * name, const char * pathname, const char * key,const char * value)
{
  return add_to_dict_cc(name,pathname,key,value); 
}

int add_to_dict_2(const char * name, const char * pathname, const char * key,const int value)
{
  return add_to_dict_2_cc(name,pathname,key,value);
}

int add_to_dict_3(const char * name, const char * pathname, const int key,char * value)
{
  return add_to_dict_3_cc(name,pathname,key,value);
}

int add_to_dict_4(const char * name, const char * pathname, const int key, const int value)
{
  return add_to_dict_4_cc (name, pathname, key,value);
}


//--------------------------------------------------------------------------

int add_batch_to_dict_1(const char * name, const char * pathname, char ** batch_key, char ** batch_value, int batch_size)
{
  return add_batch_to_dict_cc(name,pathname,batch_key, batch_value, batch_size);
}

int add_batch_to_dict_2(const char * name, const char * pathname, char ** batch_key,  int * batch_value, int batch_size)
{
  return add_batch_to_dict_2_cc(name,pathname,batch_key, batch_value, batch_size);
}

int add_batch_to_dict_3(const char * name, const char * pathname, int * batch_key, char ** batch_value, int batch_size)
{
  return add_batch_to_dict_3_cc(name,pathname,batch_key, batch_value, batch_size);
}

int add_batch_to_dict_4(const char * name, const char * pathname, int * batch_key, int * batch_value, int batch_size)
{
  return add_batch_to_dict_4_cc(name,pathname,batch_key, batch_value, batch_size);
}

//----------------------------------------------------------------------------


int get_from_dict_1(const char * name, const char * pathname, const char * key,char * value, int value_size, int *key_exists)
{
  return get_from_dict_cc(name,pathname,key,value,value_size, key_exists);
}

int get_from_dict_2(const char * name, const char * pathname, const char * key,int * value, int * key_exists)
{
  return get_from_dict_2_cc(name,pathname,key,value, key_exists);
}

int get_from_dict_3(const char * name, const char * pathname, const int key,char * value,int value_size, int* key_exists)
{
  return get_from_dict_3_cc(name,pathname,key,value,value_size, key_exists); 
}

int get_from_dict_4(const char * name, const char * pathname, const int key, int * value, int * key_exists)
{
  return get_from_dict_4_cc(name, pathname, key, value, key_exists);
}

//----------------------------------------------------------------------------


int get_first_key_val_from_dict_1(const char * name, const char * pathname, char * key, const int key_size,  char * value, const int value_size, int * first_exists)
{
  return get_first_key_val_from_dict_cc(name, pathname,  key, key_size, value, value_size, first_exists);
}

int get_first_key_val_from_dict_2(const char * name, const char * pathname,  char * key, const int key_size,  int * value, int * first_exists)
{
  return get_first_key_val_from_dict_2_cc(name, pathname,  key, key_size, value, first_exists);
}

int get_first_key_val_from_dict_3(const char * name, const char * pathname,  int * key,  char * value, const int value_size, int * first_exists)
{
  return get_first_key_val_from_dict_3_cc(name, pathname, key, value, value_size, first_exists);
}

int get_first_key_val_from_dict_4(const char * name, const char * pathname,  int * key, int * value, int * first_exists)
{
  return get_first_key_val_from_dict_4_cc(name, pathname,  key, value, first_exists);
}



int get_next_key_val_from_dict_1(const char * name, const char * pathname,  char * current_key, char * key, const int key_size,  char * value, const int value_size, int * next_exists)
{
  return get_next_key_val_from_dict_cc(name, pathname, current_key, key, key_size, value, value_size, next_exists);
}

int get_next_key_val_from_dict_2(const char * name, const char * pathname,  char * current_key, char * key, const int key_size,  int * value, int * next_exists)
{
  return get_next_key_val_from_dict_2_cc(name, pathname, current_key, key, key_size, value, next_exists);
}

int get_next_key_val_from_dict_3(const char * name, const char * pathname,  int current_key, int * key,  char * value, const int value_size, int * next_exists)
{
  return get_next_key_val_from_dict_3_cc(name, pathname, current_key, key, value, value_size, next_exists);
}

int get_next_key_val_from_dict_4(const char * name, const char * pathname,  int current_key, int * key, int * value, int * next_exists)
{
  return get_next_key_val_from_dict_4_cc(name, pathname, current_key, key, value, next_exists);
}


//----------------------------------------------------------------------------

int del_from_dict_1(const char * name, const char * pathname, const char * key)
{
  return del_from_dict_cc(name,pathname,key);
}

int del_from_dict_2(const char * name, const char * pathname, const char * key)
{
  return del_from_dict_cc(name,pathname,key); // same as prev
}

int del_from_dict_3(const char * name, const char * pathname, const int key)
{
  return del_from_dict_3_cc(name,pathname,key);
}

int del_from_dict_4(const char * name, const char * pathname, const int key)
{
  return del_from_dict_3_cc(name,pathname,key); // same as prev
}

//---------------------------------------------------------------------------- 

void cleanUp()
{
  cleanUp_cc();
}


