extern int
prep_vals(
	  char *docroot,
	  sqlite3 *db,
	  char *tbl,
	  char *fld,
	  long long nR, /* number of values */
	  long long *ptr_nn_nR, /* number of non-null values */
	  FLD_META_TYPE val_meta, // used only for source, not dest
	  FLD_META_TYPE nn_val_meta, // used only for source, not dest
	  FLD_META_TYPE lnk_meta,
	  FLD_META_TYPE nn_lnk_meta,
	  char **ptr_cpy_lnk_X,
	  size_t *ptr_cpy_lnk_nX,
	  char **ptr_cpy_idx_X,
	  size_t *ptr_cpy_idx_nX,
	  int *ptr_sizeof_idx,
	  char **ptr_cpy_lnk_file,
	  char **ptr_cpy_idx_file
	  )
;

