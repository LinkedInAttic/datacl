char *g_tbl_X = NULL;
size_t g_tbl_nX = 0;
TBL_REC_TYPE *g_tbls = NULL;
int g_n_tbl = 0;

char *g_ht_tbl_X = NULL;
size_t g_ht_tbl_nX = 0;
META_KEY_VAL_TYPE *g_ht_tbl = NULL; /* hash table for tbls */
int g_n_ht_tbl; /* sizeof above */

char *g_fld_X = NULL;
size_t g_fld_nX = 0;
FLD_REC_TYPE *g_flds = NULL;
int g_n_fld;

char *g_ht_fld_X = NULL;
size_t g_ht_fld_nX = 0;
META_KEY_VAL_TYPE *g_ht_fld = NULL; /* hash table for flds */
int g_n_ht_fld = 0; /* sizeof above */

char *g_ddir_X = NULL;
size_t g_ddir_nX = 0;
DDIR_REC_TYPE *g_ddirs = NULL;
int g_n_ddir = 0;

