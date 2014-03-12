#include "LevelDict.h"
#include <tr1/unordered_map>
#define MAX_PATH_SIZE 2048


using namespace std;
/**
 * Single Class which Encapulated open sessions in the current context.
 * This will help us optimize performance by not repeatedly writing stuff
 * disk.
**/
class DBManager
{
  private:

  protected:
  DBManager();
  ~DBManager();
  char _fullName[MAX_PATH_SIZE];
  std::tr1::unordered_map <string, LevelDict *> dbMap;
  static DBManager * _dbManager;

  public:
  static DBManager * getInstance();
  char * getFullName(const char * name,const char * pathname);
  LevelDict * getOrCreate(const char * name,const char * pathname); // Gets an open session or creates and returns it //
  void remove(const char * name,const char * pathname); // removes and deletes the dict object //

  // Call this function at end of program to close all db connections //
  int cleanUp();
};
