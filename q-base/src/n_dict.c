#include "qtypes.h"
#include "../leveldb/dictionary.h"
#include "n_dict.h"

extern char *g_data_dir;
// START FUNC DECL
int
n_dict(
    const char *dict,
    long long *ptr_n_dict
    )
// STOP FUNC DECL
{
  int status = 0;
  int dict_id;

  *ptr_n_dict = LLONG_MIN;
  status = leveldb_init(); cBYE(status);
  status = is_dictionary(g_data_dir, dict, &dict_id);
  open_dictionary(g_data_dir, dict, dict_id);
  status = n_dictionary(dict_id, ptr_n_dict); cBYE(status);
  
BYE:
  leveldb_finalize();
  return(status);
}

