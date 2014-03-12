#include "qtypes.h"
#include "../leveldb/dictionary.h"
#include "list_dicts.h"

extern char *g_data_dir;
// START FUNC DECL
int
xlist_dicts(
    )
// STOP FUNC DECL
{
  int status = 0;

  status = leveldb_init(); cBYE(status);
  status = list_dicts(); cBYE(status);
BYE:
  leveldb_finalize();
  return(status);
}

