extern int
orphan_files(
	 char *docroot,
	 sqlite3 *in_db,
	 char *indir
	 )
;
extern int
files_in_docroot(
	    char *docroot,
	    sqlite3 *db,
	    char ***ptr_files,
	    int *ptr_num_files
	    )
;

