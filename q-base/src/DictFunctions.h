#include <stdio.h>
#include "constants.h"
// cpp apis for leveldb //

#ifdef __cplusplus
extern "C" {
#endif

  //--------------------------------------------------------------------------

  int mk_dict_cc(const char * name, const char * pathname);
  int del_dict_cc(const char * name,const char * pathname);
  int is_dict_cc(const char * name, const char * pathname, int * dict_exists);

  //--------------------------------------------------------------------------

  int add_to_dict_cc(const char * name, const char * pathname, const char * key,const char * value);
  int add_to_dict_2_cc(const char * name, const char * pathname, const char * key,const int value);
  int add_to_dict_3_cc(const char * name, const char * pathname, const int key,char * value);
  int add_to_dict_4_cc(const char * name, const char *pathname, const int key, const int value);

  //------------------------------------------------------------------------

  int add_batch_to_dict_cc(const char * name, const char * pathname, char ** batch_key, char ** batch_value, int batch_size);
 int add_batch_to_dict_2_cc(const char * name, const char * pathname, char ** batch_key, int * batch_value, int batch_size);
 int add_batch_to_dict_3_cc(const char * name, const char * pathname, int * batch_key, char ** batch_value, int batch_size);
 int add_batch_to_dict_4_cc(const char * name, const char * pathname, int * batch_key, int * batch_value, int batch_size);

  //--------------------------------------------------------------------------

  int get_from_dict_cc(const char * name, const char * pathname, const char * key,char * buffer, int buffer_size, int *key_exists);
  int get_from_dict_2_cc(const char * name, const char * pathname, const char * key,int * value, int* key_exists);
  int get_from_dict_3_cc(const char * name, const char * pathname, const int key,char * buffer,int buffer_size, int *key_exists);
  int get_from_dict_4_cc(const char * name, const char * pathname, const int key, int * value, int * key_exists);

  //--------------------------------------------------------------------------

  int get_first_key_val_from_dict_cc( const char * name, const char * pathname, char * key, const int key_size,char * value, const int value_size, int * first_exists);

  int get_first_key_val_from_dict_2_cc(const char * name, const char * pathname, char * key, const int key_size,  int * value, int * first_exists);

  int get_first_key_val_from_dict_3_cc(const char * name, const char * pathname, int * key,  char * value, const int value_size, int * first_exists);

  int get_first_key_val_from_dict_4_cc(const char * name, const char * pathname, int * key, int * value, int * first_exists);



  int get_next_key_val_from_dict_cc( const char * name, const char * pathname, char * current_key, char * key, const int key_size, char * value, const int value_size, int * next_exists);

  int get_next_key_val_from_dict_2_cc(const char * name, const char * pathname,  char * current_key, char * key, const int key_size,  int * value, int * next_exists);

  int get_next_key_val_from_dict_3_cc(const char * name, const char * pathname,  int current_key, int * key,  char * value, const int value_size, int * next_exists);

  int get_next_key_val_from_dict_4_cc(const char * name, const char * pathname,  int current_key, int * key, int * value, int * next_exists);

  //--------------------------------------------------------------------------

  int del_from_dict_cc(const char * name, const char * pathname, const char * key);
  int del_from_dict_3_cc(const char * name, const char * pathname, const int key);

  //--------------------------------------------------------------------------

  int cleanUp_cc();

#ifdef __cplusplus
}
#endif
