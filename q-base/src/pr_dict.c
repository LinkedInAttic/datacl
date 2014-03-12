#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "macros.h"
#include "../leveldb/ldb_dictionary.h"
#include "../leveldb/dictionary.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

extern char *g_data_dir;

// START FUNC DECL
int pr_dict(
    const char *dict,
    const char *mode,
    const char *opfile
    )
// STOP FUNC DECL
{
  int status = 0;
  int dict_id;

  if ( g_data_dir == NULL ) { return(-1); }
  if ( dict == NULL ) { return(-1); }
  if ( opfile == NULL ) { return(-1); }

  status = leveldb_init(); cBYE(status);
  status = is_dictionary(g_data_dir, dict, &dict_id); cBYE(status);
  if ( dict_id < DICTIONARY_ID_SEED ) { go_BYE(-1); }
  open_dictionary(g_data_dir, dict, dict_id);
  if ( strcmp(mode, "csv") == 0 ) { 
    printDictionary(dict_id, opfile);
  }
  else if ( strcmp(mode, "hashbin") == 0 ) { 
    printDictionaryBin(dict_id, opfile);
  }
  else { go_BYE(-1); }
BYE:
  leveldb_finalize();
  return(status);
}
