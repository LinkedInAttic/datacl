extern int
get_outer_lb_ub(
		long long nR,
		int nT,
		int t, /* thread index */
		long long *ptr_lb,
		long long *ptr_ub
		)
;
extern int
get_num_blocks(
	       long long nR, /* number of elements to process */
	       int nC,  /* number of elements to process in one block */
	       long long *ptr_nB
	       )
;
extern int
get_num_threads(
    int *ptr_nT
    )
;

