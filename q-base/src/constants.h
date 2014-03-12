#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#define CREATE_OK  0
#define CREATE_FAILURE -1
#define DELETE_OK 0
#define DELETE_FAILURE -1
#define INSERT_OK 0
#define INSERT_FAILURE -1
#define FETCH_OK 0
#define FETCH_FAILURE -1
#define DICT_REMOVE_OK 0
#define DICT_REMOVE_FAILURE -1
#define DICT_EXISTS_OK 0
#define DICT_EXISTS_FAILURE -1
#define LEVELDB_AUTOCLEANUP 1
#define ABORT_ON_FAILURE 0

/*
 * LDB_MAX_VAL_LEN: when the value is a string, it will take a maximum of
 * these many characters
 * LDB_MAX_KEY_LEN: when the key is a string, it will take a maximum of
 * these many characters
 * */
#define LDB_MAX_STR_LEN 40
#define LDB_MAX_KEY_LEN 40
#define LDB_MAX_VAL_LEN 40

#define TRUE 1
#define FALSE 0

#define MAX_NUM_SCALARS 128 
#define OPT_NUM_SCALARS 8 /* above this we switch to hash implementation */
/* Note that current impementation assumes that sqr(MAX_NUM_SCALARS) < 65536 */
#define AUX_FLD_NN 1
#define AUX_FLD_SZ 2

#define MAX_LEN_STR     32765 // need to leave some space
#define MAX_NUM_TBLS 	256  // cannot exceed 32767 
#define MAX_NUM_FLDS_IN_TBL 1024
#define MAX_NUM_FLDS 	8192 // cannot exceed 32767 
#define MAX_NUM_FLDS_TO_LOAD 	256 // cannot exceed MAX_NUM_FLDS
#define MAX_NUM_DDIRS	8
#define MAX_LEN_SERVER_NAME    63
#define MAX_LEN_DIR_NAME       511
#define MAX_LEN_FILE_NAME      15

#define MAX_LEN_EXT_FILE_NAME 1023
#define MAX_LEN_FLDTYPE 8

#define MAX_LEN_FLD_NAME       31
#define MAX_LEN_FLD_DISP_NAME  127

#define MAX_LEN_TBL_NAME       31
#define MAX_LEN_TBL_DISP_NAME  127

#define MJOIN_OP_SUM    100
#define MJOIN_OP_MIN    200
#define MJOIN_OP_MAX    300
#define MJOIN_OP_AVG    400
#define MJOIN_OP_REG    500
#define MJOIN_OP_OR     600
#define MJOIN_OP_AND    700
#define MJOIN_OP_CNT    800

#define IOP_ADD         100
#define IOP_SUB         200
#define IOP_MUL         300
#define IOP_DIV         400
#define IOP_GT          500
#define IOP_LT          600
#define IOP_GEQ         700
#define IOP_LEQ         800
#define IOP_NEQ         900
#define IOP_EQ         1000

#define BOP_OR         101
#define BOP_AND        201
#define BOP_NOT        301

#define CACHE_SIZE 1048576
#define DEFAULT_NUM_THREADS 24
#define MIN_ROWS_FOR_PARALLELISM 1048576

#define MAX_BINS_FOR_PERCENTILE 1000

#define MAX_NUM_ARGS 16
#define MAX_LEN_ARG  63

#define NUM_BITS_IN_I1 8
#define NUM_BITS_IN_I8 64

#define DICT_READ_MODE_SAFE  1000
#define DICT_READ_MODE_QUICK 2000
#define DICT_READ_MODE_REUSE 3000

#define MAX_SHIFT 16
#define MAX_SMEAR 64
#define MAX_PACK_FLDS 8
#define MAX_DISTINCT_ENTRIES_COUNT_HT (4*1048576)

#define MAX_LEN_SCALAR_EXPRESSION 63
#define COMP_EXPR_BLOCK_SIZE 1048576
#define MAX_STATEMENTS_IN_COMP_EXPR 32

#endif
