extern int
chk_num_rows_1(
	     sqlite3 *db,
	     int tbl_id,
	     long long *ptr_nR
	     )
;
extern int
chk_num_rows_2(
    char *fldtype, 
    char *filename, 
    long long nR
    )
;
extern int
chk_num_rows_3(
    char *auxtype, 
    char *filename, 
    long long nR,
    int *ptr_n_sizeof
    )
;

