#include "DBManager.h"
#include "constants.h"
#include <stdlib.h>

void LevelDB_Clean()  { DBManager::getInstance()->cleanUp(); }  // Calls CleanUp 
DBManager * DBManager::_dbManager = NULL;
DBManager::DBManager() 
{
  if(LEVELDB_AUTOCLEANUP) atexit(LevelDB_Clean);
}
DBManager::~DBManager() {}

DBManager * DBManager::getInstance()
{
  if(!_dbManager)
  {
    _dbManager = new DBManager();
  }
  return _dbManager;
}

char *DBManager::getFullName(const char * name,const char * pathname)
{
  strcpy(_fullName,pathname);
  strcpy(_fullName+strlen(pathname),name);
  return _fullName;
}

LevelDict * DBManager::getOrCreate(const char * name,const char * pathname)
{
  char * fullName = getFullName(name,pathname);
  LevelDict * levelDict = dbMap[fullName];
  if(!levelDict)
  {
    string * fName = new std::string(fullName);
    levelDict = new LevelDict(name,pathname);
    dbMap[*fName] = levelDict;
  }
  return levelDict;
}

void DBManager::remove(const char * name,const char * pathname)
{
  char * fullName = getFullName(name,pathname);
  LevelDict * levelDict = dbMap[fullName];
  if(levelDict)
  {
    dbMap.erase(fullName);
    delete levelDict;
  }
}

/**
 * Before We close the program, call cleanup to close active dataBase Connections
**/
int DBManager::cleanUp()
{
  int status = 0;
  std::tr1::unordered_map<std::string, LevelDict *>::iterator iter = dbMap.begin();
  while(iter!=dbMap.end())
  {
    delete iter->second;
    iter++;
  }
  dbMap.clear();
  return status;
}

