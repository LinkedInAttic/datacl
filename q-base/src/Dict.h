/**
  This class encapsulates the levelDb operations we are required to make
  The main reason to encapsulate is to add efficiency, not requiring us to open the db repeatedly.
  And allow for easy migration in future.

  @author Otkrist Gupta
**/
class Dict
{
  protected:
  
  const char * _pathname;
  const char * _name;
  const char * _fullname;
  
  public :

  // Create Dict //
  Dict() {} 

  // Get Identifiers//
  const char * getName();
  const char * getFullName();

  // Creates a fullName (concatenated pathname and fullname).
  const char * getFullName(const char * name,const char * pathname);

  // Check If the object is initialized //
  bool isInitialized();

  // Create a new dictionary on hard disk //
  int init();
  int exists(int * dict_exists); // Test Existence //
  int remove(); // Delete //

  int put(const char * key,const char * value);
  int put(const char * key, const int value);
  int put(const int key, const char * value);
  int put(const int key, const int value);

  int writebatch(char ** batch_key, char ** batch_val, int batch_size);
  int writebatch(char ** batch_key, int * batch_val, int batch_size);
  int writebatch(int * batch_key, char ** batch_val, int batch_size);
  int writebatch(int * batch_key, int* batch_val, int batch_size);


  // Remove a key from dictionary //
  int removeKey(const char * key);
  int removeKey(const int key);

  // Copy the value from dictionary //
  int get(const char * key, char * buffer, int buffer_size, int * key_exists);
  int get(const char * key, int * value, int * key_exists);
  int get(const int key, char * buffer, int buffer_size, int * key_exists);
  int get(const int key, int * value, int * key_exists);

  int getfirst( char * key, const int key_size, char * value, const int value_size, int * first_exists);
  int getfirst(char * key, const int key_size, int * value, int * first_exists);
  int getfirst(int * key,char * value, const int value_size, int *first_exists);
  int getfirst(int * key, int * value, int * first_exists); 


  int getnext(char * current_key, char * key, const int key_size, char * value, const int value_size, int * next_exists);
  int getnext(char * current_key, char * key, const int key_size, int * value, int * next_exists);
  int getnext(int current_key, int * key, char * value, const int value_size, int * next_exists);
  int getnext(int current_key, int * key, int * value, int * next_exists); 

  
  ~Dict() {}
};
