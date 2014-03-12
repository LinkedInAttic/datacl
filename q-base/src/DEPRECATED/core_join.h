extern int
core_join_I_I_I_I_I_I(
			  int *src_lnk,
			  int *src_idx,
			  int *src_val,
			  long long src_nR,
			  long long nn_src_nR,
			  int *dst_lnk,
			  int *dst_idx,
			  int *dst_val,
			  char *nn_dst_val,
			  long long dst_nR,
			  long long nn_dst_nR,
			  int ijoin_op,
			  bool *ptr_is_any_null
			  );
