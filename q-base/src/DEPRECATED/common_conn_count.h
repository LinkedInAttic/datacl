extern int
find_common_conns(
		  int *X1,
		  int n1,
		  int *X2,
		  int n2,
		  int *ptr_n
		  );
extern int
int_common_conn_count(
		      int *TM_mid,
		      long long *TC_lb,
		      long long *TC_ub,
		      long long nTM,
		      int *TC_from,
		      int *TC_to,
		      int *TC_common_conns,
		      long long nTC
		      );
extern int
common_conn_count(
    char *TM,
    char *TM_mid,
    char *TM_TC_lb,
    char *TM_TC_ub,
    char *TC,
    char *TC_from,
    char *TC_to,
    char *TC_common_conns
    );
