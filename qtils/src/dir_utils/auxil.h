/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
extern int
stob(
    char *X,
    bool *ptr_Y
    )
;
extern int
stoi(
    char *X,
    int *ptr_Y
    )
;
extern int
break_str(
    const char *X,
    char *delim,
    char ***ptr_Y,
    int *ptr_nY
    )
;
extern int
num_lines(
    char *infile,
    int *ptr_num_lines
    )
;
extern int
explode(
    const char *in_X,
    const char delim,
    char ***ptr_Y,
    int *ptr_nY
    )
;
extern int
extract_name_value(
    char *in_str,
    const char *start,
    const char *stop,
    char **ptr_val
    )
;
extern int
qd_uq_str( /* quick and dirty unique string */
    char *str,
    int len
    )
;

extern long long 
mk_magic_val(
    )
;
extern int delete_directory(
    char *dir_to_del
    )
;
extern int
setup_for_bvec_unpack(
    long long n,
    char **ptr_out, 
    int *ptr_num_blocks,
    int *ptr_block_size
    );
extern bool
is_prime(
    unsigned int n
    );
extern unsigned int
prime_geq(
    int n
    );
extern bool file_exists (
    const char *filename
    );
extern int
read_nth_val( /* n starts from 0 */
    const char *in_str,
    char delim,
    int n,
    char *out_str,
    int len_out_str
    );
extern int
      strip_trailing_slash(
	  const char *in_str, 
	  char *out_str,
	  int out_len
	  );
extern int
get_disk_space ( 
    char * dev_path,
    unsigned long long *ptr_nbytes,
    char *mode
    );
