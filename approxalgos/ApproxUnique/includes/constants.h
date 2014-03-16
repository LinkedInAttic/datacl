#ifndef __CONSTANTS_H
#define __CONSTANTS_H
#define TRUE 1
#define FALSE 0

#define MAX_NUM_SCALARS 32
#define AUX_FLD_NN 1
#define AUX_FLD_SZ 2

#define MAX_NUM_TBLS 	256  // cannot exceed 32767 
#define MAX_NUM_FLDS 	8192 // cannot exceed 32767 
#define MAX_NUM_DDIRS	8
#define MAX_LEN_DIR_NAME       511
#define MAX_LEN_FILE_NAME      15

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

#define MAX_NUM_ARGS 13

#define NUM_BITS_IN_I1 8
#define NUM_BITS_IN_I8 64

#define DICT_READ_MODE_SAFE  1000
#define DICT_READ_MODE_QUICK 2000
#define DICT_READ_MODE_REUSE 3000

#endif
