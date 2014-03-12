#include "constants.h"
#include "LevelDict.h"
#include <stdlib.h>
#include "leveldb/cache.h"
#include <iostream>
#include "leveldb/db.h"
#include <string>
#include <sstream>
#include <stdio.h>
#include "leveldb/write_batch.h"
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

using namespace std;
/**
  This class encapsulates the levelDb operations we are required to make
  The main reason to encapsulate is to add efficiency, not requiring us to open the db repeatedly.
  And allow for easy migration in future.

  @author Otkrist Gupta
**/



LevelDict::LevelDict(const char * name,const char * pathname)
{
  _name = name;
  _pathname = pathname;
  _fullname = getFullName(name,pathname);
  db = NULL;
}

/**
 * Creates a fullName (concatenated pathname and fullname).
 * 
**/
char * LevelDict::getFullName(const char * name,const char * pathname)
{
  char *fullname = NULL;
  fullname = (char *)malloc(strlen(name)+strlen(pathname) + 8);
  strcpy(fullname,pathname);
  strcat(fullname,name);
  return fullname;
}

const char * LevelDict::getName() { return _name; }
const char * LevelDict::getFullName() { return _fullname; }
  
bool LevelDict::isInitialized() { return db!=NULL; }

//-----------------------------------------------------------------------

/**
* Initializes. Creates a new dict.
**/
int LevelDict::init()
{
  if(!isInitialized())
  {
    leveldb::Options options;
    options.create_if_missing = true;
    options.write_buffer_size = 200*1048576; // 200 MB of buffer
    //options.block_cache = leveldb::NewLRUCache (1048576);
    leveldb::Status status = leveldb::DB::Open(options, _fullname, &db);
    if(!status.ok()) {return CREATE_FAILURE;}
  }
  return CREATE_OK;
}

int LevelDict::exists()
{

  if(!isInitialized())
  {
    leveldb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    leveldb::Status status = leveldb::DB::Open(options, _fullname, &db);

    if ( !status.ok() ) {  return DICT_EXISTS_FAILURE; }
  } 
  return DICT_EXISTS_OK; 
}


int LevelDict::exists(int * dict_exists)
{
  *dict_exists = 0;

  if(!isInitialized())
  {
    leveldb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;
    leveldb::Status status = leveldb::DB::Open(options, _fullname, &db);

    if ( !status.ok() ) 
      {
	return DICT_EXISTS_OK; 
      }
    else 
      {
	*dict_exists = 1;
	return DICT_EXISTS_OK;
      }
  } 
  
  *dict_exists = 1;
  return DICT_EXISTS_OK; 
}

int LevelDict::remove()
{
  if(isInitialized())
  {
    delete db;
    db = NULL;
  }

  if(!isInitialized())
  {
    leveldb::Options options;
    leveldb::Status status = leveldb::DestroyDB(_fullname,options);
    if(status.ok()) return DICT_REMOVE_OK;    
  }
  return DICT_REMOVE_FAILURE;
}

//--------------------------------------------------------------------------


int LevelDict::put(const char * key, const char * value)
{

  if ( exists() == DICT_EXISTS_OK && key != NULL && value != NULL )
  {
    leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value); 
    return status.ok() ? INSERT_OK : INSERT_FAILURE;
  }
  return INSERT_FAILURE;

}


int LevelDict::put(const char * key,const int valueInt)
{

  char buffer[LDB_MAX_VAL_LEN];
  sprintf(buffer, "%d", valueInt);

  if( exists() == DICT_EXISTS_OK && key != NULL && buffer != NULL )
  {
    leveldb::Status status = db->Put(leveldb::WriteOptions(), key, buffer);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;
  }
  return INSERT_FAILURE;
}


int LevelDict::put(const int key,const char * value)
{

  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer, "%d", key);

  if( exists() == DICT_EXISTS_OK && buffer != NULL && value != NULL )
  {
    leveldb::Status status = db->Put(leveldb::WriteOptions(), buffer, value);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;
  }
  return INSERT_FAILURE;

}


int LevelDict::put(const int key,const int value)
{

  char buffer1[LDB_MAX_KEY_LEN];
  sprintf(buffer1, "%d", key);

  char buffer2[LDB_MAX_VAL_LEN];
  sprintf(buffer2, "%d", value);

  if( exists() == DICT_EXISTS_OK && buffer1 != NULL && buffer2 != NULL )
  {
    leveldb::Status status = db->Put(leveldb::WriteOptions(),buffer1,buffer2 );
    return status.ok() ? INSERT_OK : INSERT_FAILURE;
  }
  return INSERT_FAILURE;

}

//--------------------------------------------------------------------------


int LevelDict::writebatch(char ** batch_key, char ** batch_val, int batch_size)
{

  if( exists() == DICT_EXISTS_OK )
  {
 
    leveldb::WriteBatch batch;
    
    for ( int ii = 0; ii < batch_size; ii++ )
    {
      if ( batch_key[ii] != NULL && batch_val[ii] != NULL )
	batch.Put(batch_key[ii], batch_val[ii]);
      else
	return INSERT_FAILURE;
    }
    leveldb::Status status =db->Write(leveldb::WriteOptions(), &batch);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;   

  }

  return INSERT_FAILURE;

}


int LevelDict::writebatch(char ** batch_key, int * batch_val, int batch_size)
{
  
  char buffer[LDB_MAX_VAL_LEN];

  if( exists() == DICT_EXISTS_OK )
  {
 
    leveldb::WriteBatch batch;
    
    for ( int ii = 0; ii < batch_size; ii++ )
    {
      if ( batch_key[ii] != NULL )
      {
	sprintf(buffer,"%d",batch_val[ii]);
	batch.Put(batch_key[ii], buffer);
      }
      else
	return INSERT_FAILURE;
    }
    leveldb::Status status =db->Write(leveldb::WriteOptions(), &batch);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;   

  }

  return INSERT_FAILURE;

}


int LevelDict::writebatch(int * batch_key, char ** batch_val, int batch_size)
{

  char buffer[LDB_MAX_KEY_LEN];

  if( exists() == DICT_EXISTS_OK )
  {
 
    leveldb::WriteBatch batch;
    
    for ( int ii = 0; ii < batch_size; ii++ )
    {
      if ( batch_val[ii] != NULL )
      {
	sprintf(buffer,"%d",batch_key[ii]);
	batch.Put(buffer, batch_val[ii]);
      }
      else
	return INSERT_FAILURE;
    }
    leveldb::Status status =db->Write(leveldb::WriteOptions(), &batch);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;   

  }

  return INSERT_FAILURE;

}


int LevelDict::writebatch(int * batch_key,  int * batch_val, int batch_size)
{

  char buffer1[LDB_MAX_KEY_LEN];
  char buffer2[LDB_MAX_VAL_LEN];

  if( exists() == DICT_EXISTS_OK )
  {
 
    leveldb::WriteBatch batch;
    
    for ( int ii = 0; ii < batch_size; ii++ )
    {
     
       sprintf(buffer1,"%d",batch_key[ii]);
       sprintf(buffer2,"%d",batch_val[ii]);
       batch.Put(buffer1, buffer2);
 
    }
    leveldb::Status status =db->Write(leveldb::WriteOptions(), &batch);
    return status.ok() ? INSERT_OK : INSERT_FAILURE;   

  }

  return INSERT_FAILURE;

}

//--------------------------------------------------------------------------


int LevelDict::get(const char * key, char * value, int value_size, int *key_exists)
{
 
  *key_exists = 0;

  if( exists() == DICT_EXISTS_OK && key != NULL && value != NULL )
  {

    std::string str;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &str);

    if ( (int) str.length() > value_size ) { return FETCH_FAILURE; }

    if( status.ok()  )
    {
      strcpy(value, str.c_str());
      *key_exists = 1;
    }

    return FETCH_OK;
 
  }

  return FETCH_FAILURE;

}


int LevelDict::get(const char * key, int * value, int * key_exists)
{

  *key_exists = 0;

  if( exists() == DICT_EXISTS_OK && key != NULL )
  {
    std::string str;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &str);

    if( status.ok() )
    {
      *value = atoi(str.c_str());
      *key_exists = 1;
    }
    
    return FETCH_OK;

  }

  return FETCH_FAILURE;

}


int LevelDict::get(const int key, char * value, int value_size, int * key_exists)
{

  *key_exists = 0;

  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer, "%d", key);

  
  if( exists() == DICT_EXISTS_OK && buffer != NULL && value != NULL )
  {

    std::string str;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), buffer, &str);

    if ( (int) str.length() > value_size ) { return FETCH_FAILURE; }

    if( status.ok() )
    {
      strcpy(value, str.c_str());
      *key_exists = 1;
    }
    
    return FETCH_OK;

  }

  return FETCH_FAILURE;

}


int LevelDict::get(const int key, int * value, int * key_exists)
{

  *key_exists = 0;

  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer, "%d", key);

  if( exists() == DICT_EXISTS_OK && buffer != NULL )
  {

    std::string str;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), buffer, &str);
    
    if( status.ok()  )
    {
      *value = atoi(str.c_str());
      *key_exists = 1;
    }

    return FETCH_OK;

  }

  return FETCH_FAILURE;

 }

//--------------------------------------------------------------------------


int LevelDict::getfirst(char * key, const int key_size, char * value, const int value_size, int * first_exists)
{

  *first_exists = 0;

  if ( exists() == DICT_EXISTS_OK )
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> SeekToFirst();
    if ( ! it->Valid() ) { *first_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    std::string str2 = it->value().ToString();
    *first_exists = 1;

    if ( (int)str1.length() > key_size || (int)str2.length() > value_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      strcpy(key, str1.c_str());
      strcpy(value, str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getfirst(char * key, const int key_size, int * value, int * first_exists)
{

  *first_exists = 0;

  if ( exists() == DICT_EXISTS_OK)
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> SeekToFirst();
    if ( ! it->Valid() ) { *first_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    std::string str2 = it->value().ToString();
    *first_exists = 1;

    if ( (int)str1.length() > key_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      strcpy(key, str1.c_str());
      *value = atoi(str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getfirst(int * key,  char * value, const int value_size, int * first_exists)
{

  *first_exists = 0;

  if ( exists() == DICT_EXISTS_OK  )
  {
    
   leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
   it -> SeekToFirst();
   if ( ! it->Valid() ) { *first_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    std::string str2 = it->value().ToString();
    *first_exists = 1;

    if ( (int)str2.length() > value_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      *key = atoi(str1.c_str());
      strcpy(value, str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getfirst(int * key,  int * value, int * first_exists)
{

  *first_exists = 0;

  if ( exists() == DICT_EXISTS_OK )
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> SeekToFirst ();
    if ( ! it->Valid() ) { *first_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    std::string str2 = it->value().ToString();

    *first_exists = 1;

    if ( it->status().ok() ) 
    {
      *key = atoi(str1.c_str());
      *value = atoi(str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


//--------------------------------------------------------------------------


int LevelDict::getnext(char * current_key, char * key, const int key_size, char * value, const int value_size, int * next_exists)
{

  *next_exists = 0;

  if ( exists() == DICT_EXISTS_OK && current_key != NULL )
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> Seek (current_key);
    if ( ! it->Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    
    if ( strcmp(str1.c_str(),current_key) == 0 ) 
    { 
      it -> Next();    
      if ( ! it-> Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }
  
      str1 = it->key().ToString();
    }

    std::string str2 = it->value().ToString();
    *next_exists = 1;

    if ( (int)str1.length() > key_size || (int)str2.length() > value_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      strcpy(key, str1.c_str());
      strcpy(value, str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getnext(char * current_key, char * key, const int key_size, int * value, int * next_exists)
{

  *next_exists = 0;

  if ( exists() == DICT_EXISTS_OK && current_key != NULL )
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> Seek (current_key);
    if ( ! it->Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    
    if ( strcmp(str1.c_str(),current_key) == 0 ) 
    { 
      it -> Next();    
      if ( ! it-> Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }  
      str1 = it->key().ToString();
    }

    *next_exists = 1;
    std::string str2 = it->value().ToString();

    if ( (int)str1.length() > key_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      strcpy(key, str1.c_str());
      *value = atoi(str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getnext(int current_key, int * key,  char * value, const int value_size, int * next_exists)
{

  *next_exists = 0;

  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer,"%d", current_key);

  if ( exists() == DICT_EXISTS_OK && buffer!=NULL )
  {
    
   leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> Seek (buffer);
    if ( ! it->Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    
    if ( strcmp(str1.c_str(),buffer) == 0 ) 
    { 
      it -> Next();    
      if ( ! it-> Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }  
      str1 = it->key().ToString();
    }

    *next_exists = 1;
    std::string str2 = it->value().ToString();

    if ( (int)str2.length() > value_size ) 
      return FETCH_FAILURE;

    if ( it->status().ok() ) 
    {
      *key = atoi(str1.c_str());
      strcpy(value, str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


int LevelDict::getnext(int current_key, int * key,  int * value, int * next_exists)
{

  *next_exists = 0;

  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer, "%d", current_key);

  if ( exists() == DICT_EXISTS_OK && buffer!=NULL )
  {
    
    leveldb::Iterator * it = db-> NewIterator(leveldb::ReadOptions());
    
    it -> Seek (buffer);
    if ( ! it->Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }
 
    std::string str1 = it->key().ToString();
    
    if ( strcmp(str1.c_str(),buffer) == 0 ) 
    { 
      it -> Next();    
      if ( ! it-> Valid() ) { *next_exists = 0; delete it; return FETCH_OK; }  
      str1 = it->key().ToString();
    }

    *next_exists = 1;
    std::string str2 = it->value().ToString();

    if ( it->status().ok() ) 
    {
      *key = atoi(str1.c_str());
      *value = atoi(str2.c_str());
    }

    delete it;
    return FETCH_OK;	
   
  }  

  return FETCH_FAILURE;

}


//--------------------------------------------------------------------------


int LevelDict::removeKey(const char * key)
{

  if( exists() == DICT_EXISTS_OK && key != NULL )
  {
    leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
    return status.ok() ? DELETE_OK : DELETE_FAILURE;
  }

  return DELETE_FAILURE;

}


int LevelDict::removeKey(const int  key)
{
 
  char buffer[LDB_MAX_KEY_LEN];
  sprintf(buffer, "%d", key);

  if( exists() == DICT_EXISTS_OK && buffer != NULL )
  {
    leveldb::Status status = db->Delete(leveldb::WriteOptions(), buffer);
    return status.ok() ? DELETE_OK : DELETE_FAILURE;
  }

  return DELETE_FAILURE;

}

//--------------------------------------------------------------------------

LevelDict::~LevelDict()
{ 
  if( db != NULL ) delete db;
  if( _fullname != NULL ) { free((void *)_fullname); }
}
