#include "qtypes.h"
#include "../leveldb/dictionary.h"
#include "del_dict.h"

extern char *g_data_dir;
// START FUNC DECL
int
del_dict(
    const char *dict
    )
// STOP FUNC DECL
{
  int status = 0;

  int dict_id = INT_MIN;
  status = leveldb_init(); cBYE(status);
  status = is_dictionary(g_data_dir, dict, &dict_id); cBYE(status);
  if ( dict_id < 0 ) { 
    return status ;
  }
  status = delete_dictionary(g_data_dir, dict, dict_id); 
  status = 0; // Ignore status
  cBYE(status);
  
BYE:
  leveldb_finalize();
  return status ;
}

