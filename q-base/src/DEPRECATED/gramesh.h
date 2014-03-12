extern int
gramesh(
    char *str_pgk_grp_id, /* ID of the page key group of interest */
    char *next_or_prev,
    char *str_nR, /* size of main table */
    char *str_num_sessions, /* the number of sessions */
    char *str_num_pgk_grp, /* number of unique page key groups */
    char *q_data_dir,
    char *lb_file, /* index into main table: lower bound of session */
    char *ub_file, /* index into main table: upper bound of session */
    char *pgk_grp_id_file,
    char *opfile_imm
    );
