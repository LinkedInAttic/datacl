#ifndef __QTYPES_H
#define __QTYPES_H
#include <stdint.h>
#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#ifdef MAC_OSX
#include "macstuff.h"
#include <i386/limits.h>
#include <malloc/malloc.h>
#else
#include <values.h>
#endif
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <ctype.h>
#include <float.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include "constants.h"
#include "macros.h"

typedef uint8_t Q_B_t;

typedef int8_t Q_I1_t;
typedef uint8_t Q_U1_t;

typedef int16_t Q_I2_t;
typedef uint16_t Q_U2_t;

typedef int32_t Q_I4_t;
typedef uint32_t Q_U4_t;

typedef int64_t Q_I8_t;
typedef uint64_t Q_U8_t;

typedef double Q_F4_t;

typedef long double Q_F8_t;

typedef enum _TBL_TYPE {
  undef_tbl,
  regular,
  ldb_dict,
  txt_lkp,
  min_max,
  first_last,
  bins
} TBL_TYPE;

typedef struct _TBL_REC_TYPE { 
  long long nR;
  bool is_temp;
  char name[MAX_LEN_TBL_NAME+1]; 
  TBL_TYPE tbltype;
} TBL_REC_TYPE;

typedef struct _DDIR_REC_TYPE { 
  char name[MAX_LEN_DIR_NAME+1]; 
  bool is_writable; 
  bool is_ram;
} DDIR_REC_TYPE; /* Data Directory */

typedef enum _JOIN_TYPE {
  join_undef,
  join_reg,
  join_sum,
  join_min,
  join_max,
  join_and,
  join_or,
  join_cnt,
  join_minidx,
  join_maxidx,
  join_exists
} JOIN_TYPE;

typedef enum _SORT_TYPE {
  unknown,
  ascending,
  descending,
  unsorted
} SORT_TYPE;

typedef enum _FLD_TYPE {
  undef_fldtype,
  B,
  I1,
  I2,
  I4,
  I8,
  F4,
  F8,
  SC, /* raw string. Must be fixed length. len must be set */
  SV, /* Variable length string. */
} FLD_TYPE;

typedef struct _GPU_REG_TYPE { 
  void *reg; // points to data on device
  size_t filesz;
  bool is_busy;
  long long nR;
  FLD_TYPE fldtype;
  char filename[MAX_LEN_DIR_NAME+63];
  char tbl[MAX_LEN_TBL_NAME+1];
  char h_fld[MAX_LEN_FLD_NAME+1];
  char d_fld[MAX_LEN_FLD_NAME+1];
} GPU_REG_TYPE;

typedef enum _AUX_TYPE {
  undef_auxtype,
  nn,
  len,
  off,
} AUX_TYPE;

// Types of compound operations
typedef enum _COMP_OP_TYPE {
  undef_comp_expr,
  op_f1s1opf2,
  op_f1f2opf3,
  op_f1opf2f3,
  op_f1opf2,
  op_f_to_s,
  op_f1f2_to_s,
  op_count,
  op_countf,
} COMP_OP_TYPE;

typedef struct _COMP_EXPR_FLD_INFO_TYPE { 
  char name[MAX_LEN_FLD_NAME+1]; 
  int fld_id;   // -1 => not created prior to compound expr
  int tbl_id;   // since all fields may not be from same table 
  long long nR; // since all fields may not be from same table 
  int is_temp;  // 1 => created during compound expr and deleted at end 
  int is_write; // 1 => fld is written, 0 => fld is read 
  int is_first; // 1 => first time fld accessed for read/write
  FLD_TYPE fldtype; 
  void *X; // NULL => undefined
  size_t nX; // cannot be 0
  void *nn_X; 
  size_t nn_nX; 
}  COMP_EXPR_FLD_INFO_TYPE ;

typedef struct _COMP_EXPR_TYPE {
  COMP_OP_TYPE op;
  COMP_EXPR_FLD_INFO_TYPE fld[4]; // upto 3 fields supported
  char scalar[MAX_LEN_SCALAR_EXPRESSION+1];
  char op_spec[MAX_LEN_SCALAR_EXPRESSION+1];
  char env_var[MAX_LEN_FLD_NAME+1];
} COMP_EXPR_TYPE;

typedef enum _FLD_INFO_MODE_TYPE {
  undef_fld_info, /* 0 is undef. This is important assumption */
  fk_fld_txt_lkp,
  fk_fld_len,
  fk_fld_off,
} FLD_INFO_MODE_TYPE;


typedef struct _FLD_INFO_TYPE { 
  char mode;
  char unused1;
  short fld_id;
  union xxx {
    short len_fld_id;
    short off_fld_id;
    short dict_fld_id;
  } xxx;
  short unused2;
} FLD_INFO_TYPE;

typedef struct _FLD_REC_TYPE { 
  short tbl_id; /* fk to Table tbl */
  short ddir_id; /* fk to Table ddir */
  // XX short dict_tbl_id; /* if >= 0, then identifies dictionary table */
  short parent_id; 
  short nn_fld_id; /* -1 = null; else points to nn fld */
  // XX short len_fld_id; /* -1 = null; else points to len fld */
  // XX short off_fld_id; /* -1 = null; else points to off fld */
  short len; // used for clob (strings must be less than 32767)
  // This includes space for nullc termination. Hence, if string has length
  // 1, then this must be 2 

  size_t filesz;
  bool is_external; /* true => do not unlink this file */
  SORT_TYPE srttype; 
  FLD_TYPE fldtype; 
  AUX_TYPE auxtype; 

  char name[MAX_LEN_FLD_NAME+1]; 
  int fileno; 

  union sumval { 
    long long sumI8; /* Used for I1, I2, I4, I8 */
    double sumF8; /* Used for F4, F8 */
  } sumval;

  union minval { 
    long long minI8; /* Used for I1, I2, I4, I8 */
    double minF8; /* Used for F4, F8 */
  } minval;

  union maxval { 
    long long maxI8; /* Used for I1, I2, I4, I8 */
    double maxF8; /* Used for F4, F8 */
  } maxval;

  bool is_sum_nn; // tells us whether sum is defined */
  bool is_min_nn; // tells us whether min is defined */
  bool is_max_nn; // tells us whether max is defined */

  long long num_nn; // number of null values */


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
  bool is_lkp;
  bool is_trunc;
  int  maxlen;
  bool is_any_null;
  bool is_dict_old;
  bool is_null_if_missing;
} FLD_PROPS_TYPE;

#endif
