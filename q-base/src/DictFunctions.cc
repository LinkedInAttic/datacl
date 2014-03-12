#include "constants.h"
#include "DictFunctions.h"
#include "DBManager.h"

//----------------------------------------------------------------------------

int mk_dict_cc(const char * name, const char * pathname)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->init();
}

int del_dict_cc(const char * name,const char * pathname)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->remove();
}

int is_dict_cc(const char * name, const char * pathname, int * dict_exists)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->exists(dict_exists);
}

//----------------------------------------------------------------------------

int add_to_dict_cc(const char * name, const char * pathname, const char * key,const char * value)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->put(key,value);
}

int add_to_dict_2_cc(const char * name, const char * pathname, const char * key,const int value)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->put(key,value);
}

int add_to_dict_3_cc(const char * name, const char * pathname, const int key,char * value)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->put(key,value);
}

int add_to_dict_4_cc(const char * name, const char *pathname, const int key, const int value)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->put(key,value);
}

//-------------------------------------------------------------------------

int add_batch_to_dict_cc(const char * name, const char * pathname,  char ** batch_key, char ** batch_value, int batch_size)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->writebatch(batch_key, batch_value, batch_size);
}

int add_batch_to_dict_2_cc(const char * name, const char * pathname,  char ** batch_key, int * batch_value, int batch_size)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->writebatch(batch_key, batch_value, batch_size);
}

int add_batch_to_dict_3_cc(const char * name, const char * pathname,  int * batch_key, char ** batch_value, int batch_size)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->writebatch(batch_key, batch_value, batch_size);
}

int add_batch_to_dict_4_cc(const char * name, const char * pathname,   int * batch_key,  int * batch_value, int batch_size)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->writebatch(batch_key, batch_value, batch_size);
}

//---------------------------------------------------------------------------

int get_from_dict_cc(const char * name, const char * pathname, const char * key,char * buffer,int buffer_size, int *key_exists)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->get(key,buffer,buffer_size, key_exists);
}

int get_from_dict_2_cc(const char * name, const char * pathname, const char * key,int * value, int *key_exists)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->get(key,value, key_exists);
}

int get_from_dict_3_cc(const char * name, const char * pathname, const int key,char * buffer,int buffer_size, int *key_exists)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->get(key,buffer,buffer_size, key_exists);
}

int get_from_dict_4_cc(const char * name, const char * pathname, const int key, int * value, int * key_exists)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->get(key,value,key_exists);
}

//------------------------------------------------------------------------


int get_first_key_val_from_dict_cc(const char * name, const char * pathname, char * key, const int key_size, char * value, const int value_size, int * first_exists)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->getfirst( key, key_size, value, value_size, first_exists);
}

int get_first_key_val_from_dict_2_cc(const char * name, const char * pathname, char * key, const int key_size,  int * value, int * first_exists)
 {
   LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
   return d -> getfirst( key, key_size,value, first_exists);
 }

int get_first_key_val_from_dict_3_cc(const char * name, const char * pathname,  int * key,  char * value, const int value_size, int * first_exists)
  {
    LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
    return d -> getfirst( key, value, value_size, first_exists);
  }

int get_first_key_val_from_dict_4_cc(const char * name, const char * pathname,  int * key, int * value, int * first_exists)
  {
    LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
    return d -> getfirst(key,value, first_exists);
  }




int get_next_key_val_from_dict_cc(const char * name, const char * pathname, char * current_key, char * key, const int key_size, char * value, const int value_size, int * next_exists)
{
  LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->getnext(current_key, key, key_size, value, value_size, next_exists);
}

int get_next_key_val_from_dict_2_cc(const char * name, const char * pathname,  char * current_key, char * key, const int key_size,  int * value, int * next_exists)
 {
   LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
   return d -> getnext(current_key, key, key_size,value, next_exists);
 }

int get_next_key_val_from_dict_3_cc(const char * name, const char * pathname,  int current_key, int * key,  char * value, const int value_size, int * next_exists)
  {
    LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
    return d -> getnext(current_key, key, value, value_size, next_exists);
  }

int get_next_key_val_from_dict_4_cc(const char * name, const char * pathname,  int current_key, int * key, int * value, int * next_exists)
  {
    LevelDict *d = DBManager::getInstance()->getOrCreate(name,pathname);
    return d -> getnext(current_key, key,value, next_exists);
  }



//-----------------------------------------------------------------------------

int del_from_dict_cc(const char * name, const char * pathname, const char * key)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->removeKey(key);
} 

int del_from_dict_3_cc(const char * name, const char * pathname, const int key)
{
  LevelDict * d = DBManager::getInstance()->getOrCreate(name,pathname);
  return d->removeKey(key);
} 

//-----------------------------------------------------------------------------


int cleanUp_cc()
{
  return DBManager::getInstance()->cleanUp();
}




