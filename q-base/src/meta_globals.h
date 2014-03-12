extern char *g_tbl_X;
size_t extern g_tbl_nX;
extern TBL_REC_TYPE *g_tbls;
extern int g_n_tbl;

extern char *g_ht_tbl_X;
extern size_t g_ht_tbl_nX;
extern META_KEY_VAL_TYPE *g_ht_tbl; /* hash table for tbls */
extern int g_n_ht_tbl; /* sizeof above */

extern char *g_fld_X;
extern size_t g_fld_nX;
extern FLD_REC_TYPE *g_flds;
extern int g_n_fld;

extern char *g_ht_fld_X;
extern size_t g_ht_fld_nX;
extern META_KEY_VAL_TYPE *g_ht_fld; /* hash table for flds */
extern int g_n_ht_fld; /* sizeof above */

extern char *g_ddir_X;
size_t extern g_ddir_nX;
extern DDIR_REC_TYPE *g_ddirs;
extern int g_n_ddir;

extern char  *g_aux_X;
extern size_t g_aux_nX;

extern char *g_fld_info_X;
extern size_t g_fld_info_nX;
extern FLD_INFO_TYPE *g_fld_info; /* additional info about flds */
extern int g_n_fld_info; /* sizeof above */

extern int g_dsk_ddir_id;
extern int g_ram_ddir_id;
