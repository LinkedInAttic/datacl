extern int 
join(
       char *docroot,
       sqlite3 *db,
       char *src_tbl,
       char *src_lnk,
       char *src_fld,
       char *dst_tbl,
       char *dst_lnk,
       char *dst_fld
       );
extern int
simple_find_matches_if_any(
		    int *dptr, 
		    int dst_nR, 
		    int *sptr, 
		    int src_nR, 
		    int **ptr_src_idxs,
		    bool *ptr_is_any_null,
		    FILE *cfp
		    );
extern int
fast_find_matches_if_any(
		    int *dptr, 
		    int dst_nR, 
		    int *sptr, 
		    int src_nR, 
		    int **ptr_src_idxs,
		    bool *ptr_is_any_null,
		    char **ptr_nn_dptr
		    );
