#ifndef __QTYPES_H
#define __QTYPES_H
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <values.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/mman.h>
#include "constants.h"
#include "macros.h"

typedef struct _TBL_REC_TYPE { 
  long long nR;
  char name[MAX_LEN_TBL_NAME+1]; 
  char dispname[MAX_LEN_TBL_DISP_NAME+1]; 
  bool is_dict_tbl;
} TBL_REC_TYPE;

typedef struct _DDIR_REC_TYPE { 
  char name[MAX_LEN_DIR_NAME+1]; 
} DDIR_REC_TYPE; /* Data Directory */

typedef enum _SORT_TYPE {
  unknown,
  ascending,
  descending,
  unsorted
} SORT_TYPE;

typedef enum _FLD_TYPE {
  xunknown,
  B,
  I1,
  I2,
  I4,
  I8,
  F4,
  F8,
  clob,
} FLD_TYPE;

typedef enum _AUX_TYPE {
  undef,
  nn,
} AUX_TYPE;

typedef struct _FLD_REC_TYPE { 
  int tbl_id; /* fk to Table tbl */
  int ddir_id; /* fk to Table ddir */
  int is_external; /* true => do not unlink this file */
  SORT_TYPE sort_type; 
  int parent_id; 
  int nn_fld_id; /* -1 = null; else points to nn fld */
  FLD_TYPE fldtype; 
  AUX_TYPE auxtype; 
  char name[MAX_LEN_FLD_NAME+1]; 
  char filename[MAX_LEN_FILE_NAME+1]; 
  char dispname[MAX_LEN_FLD_DISP_NAME+1]; 
  int dict_tbl_id; /* fk to Table tbl */
  int alias_of_fld_id; /* fk to itself. >0 => alias of some other fld */
  long long cnt; /* >=0 => defined. Used for B and I1 when vals are 0/1 */
} FLD_REC_TYPE; 

typedef struct meta_key_val_type {
  int key;
  int val;
} META_KEY_VAL_TYPE;


typedef struct _HOP_REC_TYPE {
  unsigned long long key;
  unsigned int len;
  unsigned int orig_idx;
} HOP_REC_TYPE;

typedef struct _HT_REC_TYPE {
  char *strkey;
  unsigned long long key;
  unsigned long long cnt;
} HT_REC_TYPE;

typedef struct _HT_I8_TYPE { 
  unsigned long long key;
  unsigned long long cnt;
} HT_I8_TYPE;

typedef struct _FLD_PROPS_TYPE { 
  bool is_load;
  bool is_all_def;
  bool is_string;
  bool is_any_null;
  bool is_dict_old;
  bool is_null_null;
  bool is_null_if_missing;
} FLD_PROPS_TYPE;

#endif
