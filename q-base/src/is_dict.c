#include "qtypes.h"
#include "../leveldb/dictionary.h"
#include "is_dict.h"

extern char *g_data_dir;
// START FUNC DECL
int
is_dict(
    const char *dict,
    int *ptr_dict_id 
    )
// STOP FUNC DECL
{
  int status = 0;

  *ptr_dict_id = INT_MIN;
  status = leveldb_init(); cBYE(status);
  status = is_dictionary(g_data_dir, dict, ptr_dict_id);
  cBYE(status);
  
BYE:
  leveldb_finalize();
  return status ;
}

