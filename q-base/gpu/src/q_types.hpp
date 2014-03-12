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
#include <sys/mman.h>
// #include "constants.h"
// #include "macros.h"

#include <stdint.h>

#include <iostream>
namespace QDB {
  inline void whereami(char const * FILE, int LINE) {
    std::cerr << "Line " << LINE << " of File " << FILE << std::endl;
  }
  const int Q_PASS = 0;
  const int Q_FAIL = 1;
}

typedef uint8_t Q_B_t;

// typedef int8_t Q_I1_t;
typedef char Q_I1_t; // TEMPORARY HACK UNTIL WE CAN REPLACE char WITH Q_I1_t
typedef uint8_t Q_U1_t;

typedef int16_t Q_I2_t;
typedef uint16_t Q_U2_t;

typedef int32_t Q_I4_t;
typedef uint32_t Q_U4_t;

typedef int64_t Q_I8_t;
typedef uint64_t Q_U8_t;

typedef float Q_F4_t;
typedef double Q_F8_t;

struct TBL_FLD_TYPE {
  short tbl_id;
  short fld_id;
};

enum TBL_TYPE {
  undef_tbl,
  regular,
  txt_lkp,
  min_max,
  first_last,
  bins
};

struct TBL_REC_TYPE { 
  size_t nR;
  std::string name;
  TBL_TYPE tbl_type;
};

struct DDIR_REC_TYPE { 
  std::string name;
};

enum JOIN_TYPE {
  join_undef,
  join_reg,
  join_sum,
  join_min,
  join_max,
  join_and,
  join_or,
  join_cnt
};

enum SORT_TYPE {
  unknown,
  ascending,
  descending,
  unsorted
};

enum FLD_TYPE {
  undef_fldtype,
  B,
  I1,
  I2,
  I4,
  I8,
  F4,
  F8,
  SC, /* raw string. Must be fixed length. len must be set */
  SV /* Variable length string. dict_tbl_id must be set. */
};

enum AUX_TYPE {
  undef_auxtype,
  nn
};

struct FLD_REC_TYPE { 
  short tbl_id; /* fk to Table tbl */
  short ddir_id; /* fk to Table ddir */
  short dict_tbl_id; /* if >= 0, then identifies dictionary table */
  short parent_id; 
  short nn_fld_id; /* -1 = null; else points to nn fld */
  short len; // used for clob (strings must be less than 32767)
  // This includes space for nullc termination. Hence, if string has length
  // 1, then this must be 2 

  size_t filesz;
  bool is_external; /* true => do not unlink this file */
  SORT_TYPE srttype; 
  FLD_TYPE fldtype; 
  AUX_TYPE auxtype; 

  std::string name;
  int fileno; 

  long long sumI8; /* Used for I1, I2, I4, I8 */
  long long minI8; /* Used for I1, I2, I4, I8 */
  long long maxI8; /* Used for I1, I2, I4, I8 */

  double sumF8; /* Used for F4, F8 */
  double minF8; /* Used for F4, F8 */
  double maxF8; /* Used for F4, F8 */

  bool is_sum_nn; // tells us whether sum is defined */
  bool is_min_nn; // tells us whether sum is defined */
  bool is_max_nn; // tells us whether sum is defined */
}; 

struct  META_KEY_VAL_TYPE {
  int key;
  int val;
};

struct HOP_REC_TYPE {
  unsigned long long key;
  unsigned int len;
  unsigned int orig_idx;
};

struct HT_REC_TYPE {
  char *strkey;
  unsigned long long key;
  unsigned long long cnt;
};

struct HT_I8_TYPE {
  unsigned long long key;
  unsigned long long cnt;
};

struct FLD_PROPS_TYPE { 
  bool is_load;
  bool is_all_def;
  bool is_string;
  bool is_any_null;
  bool is_dict_old;
  bool is_null_null;
  bool is_null_if_missing;
};

#endif
