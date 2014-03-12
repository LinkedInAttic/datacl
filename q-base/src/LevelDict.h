#include "leveldb/db.h"
#include "Dict.h"
#include "constants.h"

/**
  This class encapsulates the levelDb operations we are required to make
  The main reason to encapsulate is to add efficiency, not requiring us to open the db repeatedly.
  And allow for easy migration in future.

  @author Otkrist Gupta
**/

class LevelDict: public Dict
{
  protected:
  leveldb::DB* db;

  public:
  LevelDict(const char * name,const char * pathname);
  ~LevelDict(); 
  
  const char * getName();
  const char * getFullName();
  char * getFullName(const char * name,const char * pathname);
  bool isInitialized();

  //-----------------------------------------------------------------------

  int init();
  int exists(int * dict_exists); 
  int exists();
  int remove();

  //-----------------------------------------------------------------------

  int put(const char * key,const char * value);
  int put(const char * key, const int value);
  int put(const int key, const char * value);
  int put(const int key, const int value);

  //-----------------------------------------------------------------------

  int writebatch(char ** batch_key, char ** batch_val, int batch_size);
  int writebatch(char ** batch_key, int * batch_val, int batch_size);
  int writebatch(int * batch_key, char ** batch_val, int batch_size);
  int writebatch(int * batch_key, int* batch_val, int batch_size);

  //-----------------------------------------------------------------------

  int get(const char * key, char * buffer, int buffer_size, int * key_exists);
  int get(const char * key, int * value, int * key_exists);
  int get(const int key, char * buffer, int buffer_size, int * key_exists);
  int get(const int key, int * value, int * key_exists);

  //-----------------------------------------------------------------------

  int getfirst( char * key, const int key_size, char * value, const int value_size, int * first_exists);
  int getfirst(char * key, const int key_size, int * value, int * first_exists);
  int getfirst(int * key,char * value, const int value_size, int *first_exists);
  int getfirst(int * key, int * value, int * first_exists); 

  int getnext(char * current_key, char * key, const int key_size, char * value, const int value_size, int * next_exists);
 int getnext(char * current_key, char * key, const int key_size, int * value, int * next_exists);
  int getnext(int current_key, int * key, char * value, const int value_size, int * next_exists);
  int getnext(int current_key, int * key, int * value, int * next_exists); 


  //-----------------------------------------------------------------------

  int removeKey(const char * key);
  int removeKey(const int key);

  //-----------------------------------------------------------------------


};
