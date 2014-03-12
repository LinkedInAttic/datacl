int
  count_ones(
      char *tbl,
      char *fld,
      long long *ptr_new_fld_cnt
      )
  {
    int status = 0;
    char buf1[64],buf2[64]; char *endptr;
    zero_string(buf1, 64);
    zero_string(buf2, 64);

    status = ext_f_to_s(tbl, fld, "sum", buf1,  64); cBYE(status);
    status = read_nth_val(buf1, ':', 0, buf2, 64); cBYE(status);
    *ptr_fld_cnt = strtoll(buf2, &endptr, 10); 
    if ( *endptr != '\0' ) { go_BYE(-1); }
    if ( *ptr_fld_cnt == 0 ) {
      fprintf(stderr, "WARNING! Adding fld [%s] => count = 0\n", fld);
    }
BYE:
    return(status);
  }

    if ( new_fld_cnt == fld_cnt ) { 
