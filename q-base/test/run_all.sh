#/usr/local/bin/bash
shopt -s expand_aliases
alias q='valgrind $valgrind_options q'
set -e
rm -f ${VALGRIND_LOG}.*
#---------------------------------------------
foo()
{
  if [ $# != 1 ]; then echo FAILURE; exit 1;  fi
  dir=$1
  cd $ROOTDIR 1>/dev/null 2>&1
  test -d $dir
  echo "Testing $dir"
  cd $dir 1>/dev/null 2>&1
  bash test.sh
  cd $ROOTDIR 1>/dev/null 2>&1
}

ROOTDIR=$PWD
foo f1f2opf3
foo f1opf2
foo f1s1opf2
foo fop
foo f_to_s
foo s_to_f
foo import
foo integrity
foo is_a_in_b
foo lcase
foo mk_tbl_hash_words
foo mk_tbl_hash_words.db1.sq3
foo percentiles
foo run_all.sh
foo set_val
foo srt_join
foo str_join
foo test_mjoin.sh
foo title_similarity_preproc
foo wisifxthenyelsez
foo app_tbl
foo bindmp
foo bitwise_histo
foo cli_closest_lat_long
foo copy_fld
foo count_strings
foo crossprod
foo dld
foo dup_fld
foo f1f2f3f4opf5
