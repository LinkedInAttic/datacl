extern int
elim_null_vals(
	char *docroot,
	sqlite3 *db,
	char *intbl,
	char *infld,
	char **ptr_outtbl
);
extern int
aux_join_get_vals(
	  char *docroot,
	  sqlite3 *in_db,
	  char *x_tbl,
	  char *x_lnk, /* link field */
	  char *x_idx, /* idx field */
	  char *tbl,
	  char *val, /* value to be joined */
	  FLD_META_TYPE *ptr_lnk_meta,
	  FLD_META_TYPE *ptr_val_meta,
	  FLD_META_TYPE *ptr_idx_meta,
	  long long *ptr_x_nR
	  );
extern int
sort_if_necessary(
	char *docroot, 
	sqlite3 *db, 
	char *tbl, 
	char *fld1,
	char *fld2
);
