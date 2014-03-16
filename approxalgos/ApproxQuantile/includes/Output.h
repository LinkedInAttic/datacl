#include <inttypes.h>

int
Output(
       int **src_bufs,      
       int *ptr_weight, 
       int *last_packet,
       int last_packet_incomplete,
       long long last_packet_siz, 
       long long eff_siz, 
       long long num_quantiles,
       int *dst,
       int b,         
       long long k
       );
